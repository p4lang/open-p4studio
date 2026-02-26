/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

/* bf_knet kernel module
 *
 * This is a kernel module for KNET Packet Filtering.
 * Rx filtering and Tx actions are supported
 *
 * It also allows for creation of host interfaces (mapped to device ports)
 *
 * Rx packets are demuxed to a KNET interface (cpuif_knetdev or hostif_knetdev)
 * based on filter match and action.
 *
 * Tx packets received on KNET netdevs (host interfaces) and are sent out
 * of the CPU host netdev after applying any Tx actions configured.
 *
 * This module implements P4 agnostic filtering and thus is packet header
 *structure independent.
 **/

#include <linux/init.h> // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/byteorder/generic.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <asm/atomic.h>
#include <linux/sort.h>
#include <linux/rtnetlink.h>
#include <linux/version.h>
#include <linux/notifier.h>
#include "bf_knet_priv.h"

#define MASK 0x000000ffu
#define PRINT_PACKET_INFO

static int debug = 0;

static struct list_head _device_list; // List of KNET devices (cpuif_netdev)
static atomic_t module_initialized;
static struct net_device *bf_knet;
static void bf_knet_change_host_mtu(struct net_device *netdev);

#define knet_debug(level, dev, fmt, args...)                                   \
	do {                                                                   \
		bf_knet_priv_t *knet = netdev_priv(bf_knet);                   \
		int print = 1;                                                 \
		if (strcmp(level, KERN_ERR) == 0)                              \
			dev_printk(level, dev, fmt, ##args);                   \
		else if (debug) {                                              \
			if (strcmp(level, KERN_EMERG) == 0) {                  \
				if (knet->debug_level < LOGLEVEL_EMERG) {      \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_ALERT) == 0) {           \
				if (knet->debug_level < LOGLEVEL_ALERT) {      \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_CRIT) == 0) {            \
				if (knet->debug_level < LOGLEVEL_CRIT) {       \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_WARNING) == 0) {         \
				if (knet->debug_level < LOGLEVEL_WARNING) {    \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_NOTICE) == 0) {          \
				if (knet->debug_level < LOGLEVEL_NOTICE) {     \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_INFO) == 0) {            \
				if (knet->debug_level < LOGLEVEL_INFO) {       \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_DEBUG) == 0) {           \
				if (knet->debug_level < LOGLEVEL_DEBUG) {      \
					print = 0;                             \
				}                                              \
			} else if (strcmp(level, KERN_DEFAULT) == 0) {         \
				if (knet->debug_level < LOGLEVEL_DEFAULT) {    \
					print = 0;                             \
				}                                              \
			} else                                                 \
				print = 0;                                     \
                                                                               \
			if (print)                                             \
				dev_printk(level, dev, fmt, ##args);           \
		}                                                              \
	} while (0)

static inline void printPacket(void *pkt, int pktsize)
{
	uint8_t *ptr;
	int i;
	ptr = pkt;
	for (i = 0; i < pktsize; i++)
		printk(KERN_CONT "%02x ", (*((int *)ptr++)) & MASK);
	printk(KERN_INFO "\n");
}

static inline void ppacket(char *s, struct sk_buff *skb)
{
	bf_knet_priv_t *knet = netdev_priv(bf_knet);
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	knet_debug(KERN_INFO, &skb->dev->dev,
		   "%s::%s: head:%p data:%p tail:%d end:%d headroom:%d "
		   "len:%d data_len:%d\n",
		   KBUILD_MODNAME, s, skb->head, skb->data, skb->tail, skb->end,
		   skb_headroom(skb), skb->len, skb->data_len);
#else
	knet_debug(KERN_INFO, &skb->dev->dev,
		   "%s::%s: head:%p data:%p tail:%p end:%p headroom:%d "
		   "len:%d data_len:%d\n",
		   KBUILD_MODNAME, s, skb->head, skb->data, skb->tail, skb->end,
		   skb_headroom(skb), skb->len, skb->data_len);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0)
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	if (skb_mac_header_was_set(skb)) {
		knet_debug(KERN_INFO, &skb->dev->dev, "Mac header offset:%u\n",
			   skb->mac_header);
	}
#else
	if (skb_mac_header_was_set(skb)) {
		knet_debug(KERN_INFO, &skb->dev->dev, "Mac header offset:%p\n",
			   skb->mac_header);
	}
#endif
#else /* Linux version >=3.11.xx */
	if (skb_mac_header_was_set(skb)) {
		knet_debug(KERN_INFO, &skb->dev->dev, "Mac header offset:%u\n",
			   skb->mac_header);
	}
#endif
	if (debug && (knet->debug_level >= LOGLEVEL_INFO)) {
		printPacket(((void *)(skb->data)), skb->len);
	}
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
#if defined(RHEL_RELEASE_CODE)
#else
static inline int skb_mac_offset(const struct sk_buff *skb)
{
	return skb_mac_header(skb) - skb->data;
}
#endif /* (RHEL_RELEASE_CODE) */
#endif

static inline bf_cpuif_info_t *bf_cpuif_ndev_lookup(const char *name)
{
	bf_cpuif_info_t *cpuif_info = NULL;

        if (!name) {
            knet_debug(KERN_ERR, &bf_knet->dev,
                "%s:%s:%d::Netdev name should not have been null\n",
                KBUILD_MODNAME, __func__, __LINE__);
            return NULL;        
        } 

	list_for_each_entry_rcu(cpuif_info, &_device_list, list)
	{
		if (cpuif_info->cpuif_netdev &&
	            !strcmp((cpuif_info->cpuif_netdev->name), name)) {
			return cpuif_info;
		}
	}

	return NULL;
}

static inline bf_knet_rx_filter_info_t *
bf_knet_match_rx_pkt(bf_cpuif_info_t *cpuif_info, struct sk_buff *skb)
{
	bf_knet_rx_filter_info_t *filter;
	bf_knet_rx_filter_spec_t *fspec;
	int idx;
	/* skb linear data buffer size */
	unsigned lsize;
	int match;
	uint8_t *data;

	knet_debug(KERN_INFO, &skb->dev->dev, "%s::Filter match function\n",
		   KBUILD_MODNAME);
	/* We find the length of linear section of skb data. For now filtering
	is only done on linear section of skb data buffer. If filter size is
	greater
	than skb linear data length , we skip filter matching for tha filter */
	lsize = skb->len - skb->data_len;
	data = skb->data;

	list_for_each_entry_rcu(filter, &cpuif_info->bf_knet_rx_pf_list, list)
	{
		if (filter->rx_filter.spec.filter_size < lsize) {
			fspec = &filter->rx_filter.spec;
			match = 1;
			for (idx = 0; idx < fspec->filter_size; idx++) {
				knet_debug(KERN_INFO, &skb->dev->dev,
					   "Byte[%d]: data:%u mask:%u, masked "
					   "data:%u filter:%u \n",
					   idx, data[idx], fspec->mask[idx],
					   data[idx] & fspec->mask[idx],
					   fspec->filter[idx]);
				if ((data[idx] & fspec->mask[idx]) !=
				    fspec->filter[idx]) {
					knet_debug(
					    KERN_INFO, &skb->dev->dev,
					    "Byte[%d] did not not match \n",
					    idx);
					match = 0;
					break;
				}
			}
			if (match) {
				filter->hits++;
				return filter;
			}
		} else {
			knet_debug(KERN_INFO, &skb->dev->dev,
				   "%s::Filter:%llu, size greater than "
				   "packet length skipping filter\n",
				   KBUILD_MODNAME,
				   filter->rx_filter.spec.filter_id);
		}
	}
	return NULL;
}

// TODO: Compile for different kernel versions
int bf_knet_rcv(struct sk_buff *skb, struct net_device *dev1,
		struct packet_type *pt, struct net_device *dev2)
{

	bf_cpuif_info_t *cpuif_info = NULL;
	bf_knet_rx_filter_info_t *filter = NULL;
	bf_knet_rx_filter_action_t *action = NULL;
	bf_knet_priv_t *priv = NULL;
	bf_knet_rx_filter_spec_t *fspec;
	int mac_offset = 0;
	/* Extra Headroom Needed */
	unsigned int needed_headroom = 0;
	int idx;

	if (skb->pkt_type == PACKET_OUTGOING) {
		// When the protocol handler is of type ETH_P_ALL,
		// kernel delivers all Tx skbs (clones) to these handlers as
		// well
		// as these handlers are sniffers.

		// ppacket("Outgoing Tx Packet received from
		// dev_queue_xmit_nit",skb);
		rcu_read_lock();
		goto drop;
	}

	// CPU host packet received
	if (skb_mac_header_was_set(skb)) {
		mac_offset = skb_mac_offset(skb);
		if (likely(mac_offset <= 0)) {
			mac_offset *= -1;
		} else {
			knet_debug(KERN_ERR, &dev1->dev,
				   "%s:%s:%d::SKB MAC header mac offset "
				   "wrongly set, offset = %d",
				   KBUILD_MODNAME, __func__, __LINE__,
				   mac_offset);
		}
		skb_push(skb, mac_offset);
		skb_reset_mac_header(skb);
	}
	ppacket("bf_knet_rcv: Packet received", skb);

	rcu_read_lock();
	cpuif_info = bf_cpuif_ndev_lookup(dev1->name);
	if (cpuif_info == NULL) {
		knet_debug(
		    KERN_WARNING, &dev1->dev,
		    "No context for device found in KNET, Dropping packet\n");
		goto drop;
	}

	filter = bf_knet_match_rx_pkt(cpuif_info, skb);
	if (!filter) {
		knet_debug(KERN_INFO, &dev1->dev,
			   "Rx Filter NULL, Dropping packet\n");
		goto drop;
	}

	fspec = &filter->rx_filter.spec;
	action = &filter->rx_filter.action;
	knet_debug(KERN_INFO, &dev1->dev,
		   "Packet matched filter %llu with priority %d\n",
		   fspec->filter_id, fspec->priority);
	switch (action->dest_type) {
	case BF_KNET_FILTER_DESTINATION_NULL:
		knet_debug(KERN_INFO, &dev1->dev,
			   "Filter action dest NULL,Dropping packet\n");
		goto drop;
		break;
	case BF_KNET_FILTER_DESTINATION_CPUIF:
		skb = skb_share_check(skb, GFP_ATOMIC);
		if (!skb)
			goto fail;

		skb->dev = cpuif_info->cpuif_knetdev;

		rcu_read_unlock();
		if (likely(netif_rx(skb) == NET_RX_SUCCESS))
			knet_debug(KERN_INFO, &dev1->dev,
				   "Packet transmitted to cpuif_knetdev %s\n",
				   skb->dev->name);

		priv = netdev_priv(skb->dev);
		priv->stats.rx_packets++;
		priv->stats.rx_bytes += skb->len;
		return NET_RX_SUCCESS;
		break;
	case BF_KNET_FILTER_DESTINATION_HOSTIF:
		/* Stale hostif_knetdev entry. This happens when the
		 * hostif_knetdev was deleted after the filter was added */
		if (filter->hostif_knetdev == NULL) {
			knet_debug(KERN_WARNING, &dev1->dev,
				   "Hostif_knetdev for filter id "
				   "%llu deleted, dropping packet\n",
				   fspec->filter_id);
			goto drop;
		}
		if (filter->pkt_min_len > skb->len) {
			knet_debug(KERN_INFO, &dev1->dev,
				   "Packet Length: %d less than "
				   "number of bytes required for "
				   "filter action:%d, Dropping "
				   "packet\n",
				   skb->len, filter->pkt_min_len);
			priv = netdev_priv(filter->hostif_knetdev);
			priv->stats.rx_dropped++;
			goto drop;
		}
		needed_headroom = filter->delta_net > 0 ? filter->delta_net : 0;
		/* If headroom is sufficient for new headers and the skb is
		neither a shared copy
		nor cloned, we can modify both the skb and its data buffer */
		if (needed_headroom == 0 && !skb_cloned(skb) &&
		    !skb_shared(skb)) {
			goto action;
		} else {
			/* Allocate new skb data buffer that meets headroom
			requirements.This also makes the skb writeable
			just in case the skb was shared */
			struct sk_buff *skb_orig = skb;
			knet_debug(KERN_INFO, &dev1->dev,
				   "Additional headroom needed for skb %d \n",
				   needed_headroom);
			if ((skb = skb_realloc_headroom(
				 skb, needed_headroom)) == NULL) {
				knet_debug(KERN_ERR, &bf_knet->dev,
					   "%s:%s:%d::Failed to allocated "
					   "headroom for the skb",
					   KBUILD_MODNAME, __func__, __LINE__);
				priv = netdev_priv(filter->hostif_knetdev);
				priv->stats.rx_dropped++;
				skb = skb_orig;
				goto drop;
			} else {
				dev_kfree_skb_any(skb_orig);
			}
		}
	action:
		for (idx = 0; idx < action->count; idx++) {
			if (action->pkt_mutation[idx].mutation_type ==
			    BF_KNET_RX_MUT_STRIP) {
				memmove(skb->data +
					    action->pkt_mutation[idx].len,
					skb->data,
					action->pkt_mutation[idx].offset);
				knet_debug(KERN_INFO, &dev1->dev,
					   "Headroom:%d Stripped %d "
					   "bytes off packet \n",
					   skb_headroom(skb),
					   action->pkt_mutation[idx].len);
				skb_pull(skb, action->pkt_mutation[idx].len);
			} else if (action->pkt_mutation[idx].mutation_type ==
				   BF_KNET_RX_MUT_INSERT) {
				memmove(skb->data -
					    action->pkt_mutation[idx].len,
					skb->data,
					action->pkt_mutation[idx].offset);
				knet_debug(KERN_INFO, &dev1->dev,
					   "Headroom:%d Pushing %d "
					   "bytes onto packet \n",
					   skb_headroom(skb),
					   action->pkt_mutation[idx].len);
				skb_push(skb, action->pkt_mutation[idx].len);
				memcpy(skb->data +
					   action->pkt_mutation[idx].offset,
				       action->pkt_mutation[idx].data,
				       action->pkt_mutation[idx].len);
			}
		}
		skb_reset_mac_header(skb);
		break;
	default:
		knet_debug(KERN_INFO, &dev1->dev,
			   "Invalid rx filter action: %d, Dropping packet\n", action->dest_type);
		goto drop;
	}

	if (action->dest_proto != 0) {
		skb->protocol = action->dest_proto;
	} else {
		skb->protocol = eth_type_trans(skb, dev1);
	}
	/* Kernel drops packet when destined to other hosts. But since
	we decide here to have packet received on a hostif_knetdev we
	know that the packet is for this host so we set its type as
	PACKET_HOST */
	if (skb->pkt_type == PACKET_OTHERHOST) {
		skb->pkt_type = PACKET_HOST;
	}

	skb->dev = rcu_dereference(filter->hostif_knetdev);

	rcu_read_unlock();
	ppacket("Packet being transmitted", skb);
	if (likely(netif_rx(skb) == NET_RX_SUCCESS))
		knet_debug(KERN_INFO, &skb->dev->dev,
			   "Packet transmitted to hostif_knetdev %s "
			   "with skb proto %d pkt_type %d\n",
			   skb->dev->name, skb->protocol, skb->pkt_type);
	priv = netdev_priv(skb->dev);
	priv->stats.rx_packets++;
	priv->stats.rx_bytes += skb->len;
	return NET_RX_SUCCESS;
fail:
drop:
	rcu_read_unlock();
	if (skb)
		dev_kfree_skb_any(skb);
	return NET_RX_DROP;
}

netdev_tx_t _bf_knet_tx(struct sk_buff *skb, struct net_device *dev)
{
	bf_knet_priv_t *priv = NULL;
	bf_knet_tx_action_t *action;
	int idx, needed_headroom = 0;
	int mac_offset = 0;
	bool need_free = true;
	int rc = 0;

	if (skb_mac_header_was_set(skb)) {
		mac_offset = skb_mac_offset(skb);
		if (mac_offset <= 0) {
			mac_offset *= -1;
		} else {
			knet_debug(KERN_ERR, &dev->dev,
				   "%s:%s:%d::SKB MAC header mac offset "
				   "wrongly set, offset = %d",
				   KBUILD_MODNAME, __func__, __LINE__,
				   mac_offset);
		}
		skb_push(skb, mac_offset);
		skb_reset_mac_header(skb);
	}
	ppacket("Tx packet on netdev", skb);
	priv = netdev_priv(dev);
	skb->dev = priv->cpuif_netdev;

	rcu_read_lock();
	action = rcu_dereference(priv->tx_action);
	if (action == NULL) {
		knet_debug(KERN_INFO, &dev->dev, "No tx action defined"
						 "Dropping packet\n");
		goto drop;
	}
	if (priv->pkt_min_len > skb->len) {
		knet_debug(KERN_INFO, &dev->dev,
			   "Packet Length: %d less than number of "
			   "bytes required for tx action:%d, Dropping "
			   "packet\n",
			   skb->len, priv->pkt_min_len);
		goto drop;
	}

	/* TF2 UMAC has a bug to pad RUNT packet,so pad small packet to 60
	bytes here by S/W. SKB buffer is freed inside eth_skb_pad on error.
	*/
	if (eth_skb_pad(skb))
	{
		need_free = false;
		goto drop;
	}

	needed_headroom = priv->delta_net > 0 ? priv->delta_net : 0;
	/* If headroom is sufficient for new headers and the skb is neither a
	shared copy
	nor cloned, we can modify both the skb and its data buffer */
	if (needed_headroom == 0 && !skb_cloned(skb) && !skb_shared(skb)) {
		goto action;
	} else {
		/* Allocate new skb data buffer that meets headroom
		requirements.This also makes the skb writeable
		just in case the skb was shared */
		struct sk_buff *skb_orig = skb;
		knet_debug(KERN_INFO, &dev->dev,
			   "Additional headroom needed for skb %d \n",
			   needed_headroom);
		if ((skb = skb_realloc_headroom(skb, needed_headroom)) ==
		    NULL) {
			knet_debug(
			    KERN_ERR, &dev->dev,
			    "%s:%s:%d::Failed to allocate headroom for skb",
			    KBUILD_MODNAME, __func__, __LINE__);
			skb = skb_orig;
			goto drop;
		} else {
			dev_kfree_skb_any(skb_orig);
		}
	}
action:
	for (idx = 0; idx < action->count; idx++) {

		if (action->pkt_mutation[idx].mutation_type ==
		    BF_KNET_RX_MUT_STRIP) {
			memmove(skb->data + action->pkt_mutation[idx].len,
				skb->data, action->pkt_mutation[idx].offset);
			knet_debug(
			    KERN_INFO, &dev->dev,
			    "Headroom:%d Stripped %d bytes off packet \n",
			    skb_headroom(skb), action->pkt_mutation[idx].len);
			skb_pull(skb, action->pkt_mutation[idx].len);
		} else if (action->pkt_mutation[idx].mutation_type ==
			   BF_KNET_RX_MUT_INSERT) {
			memmove(skb->data - action->pkt_mutation[idx].len,
				skb->data, action->pkt_mutation[idx].offset);
			knet_debug(
			    KERN_INFO, &dev->dev,
			    "Headroom:%d Pushing %d bytes onto packet \n",
			    skb_headroom(skb), action->pkt_mutation[idx].len);
			skb_push(skb, action->pkt_mutation[idx].len);
			memcpy(skb->data + action->pkt_mutation[idx].offset,
			       action->pkt_mutation[idx].data,
			       action->pkt_mutation[idx].len);
		}
	}

	skb_reset_mac_header(skb);
	rcu_read_unlock();

	priv->stats.tx_packets++;
	priv->stats.tx_bytes += skb->len;
	rc = dev_queue_xmit(skb);
	ppacket("Packet transmitted to cpuif_netdev", skb);
	return rc;
drop:
	priv->stats.tx_dropped++;
	if (need_free)
	{
		dev_kfree_skb_any(skb);
	}
	rcu_read_unlock();
	return NET_XMIT_DROP;
}

struct net_device_stats *_bf_knet_get_stats(struct net_device *netdev)
{
	bf_knet_priv_t *priv;

	priv = netdev_priv(netdev);
	return &priv->stats;
}
int _bf_knet_open(struct net_device *dev)
{
	netif_start_queue(dev);
	return 0;
}

int _bf_knet_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

void knetdev_destructor(struct net_device *dev)
{
	knet_debug(KERN_INFO, &dev->dev,
		   "%s::Deleting knetdev (%s): reg_state:%d\n", KBUILD_MODNAME,
		   dev->name, dev->reg_state);
	if (dev->reg_state != NETREG_RELEASED) {
		free_netdev(dev);
	}
}

static int kndev_change_carrier(struct net_device *dev, bool new_carrier)
{
	if (new_carrier)
		netif_carrier_on(dev);
	else
		netif_carrier_off(dev);
	return 0;
}

#define BF_KNET_MIN_FRAME_SIZE 68
#define BF_KNET_MAX_JUMBO_FRAME_SIZE 9728

static int
_bf_knet_change_mtu(struct net_device *netdev, int new_mtu)
{
    bf_cpuif_info_t *cpuif_info;
    bf_knet_priv_t *priv;

    //Find the corresponding CPU interface mtu
    list_for_each_entry_rcu(cpuif_info, &_device_list, list)
    {
        if (cpuif_info->cpuif_netdev) {
            list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
            {
                if (!strcmp(netdev->name, priv->hostif_knetdev->name)) {
                    if ((new_mtu > cpuif_info->cpuif_netdev->mtu)) {
                        printk(KERN_ERR DRV_NAME ": Host interface %s mtu size "
                                            "should be less than or equal to "
                                            "CPU interface %s mtu size %d\n",
                                            priv->hostif_knetdev->name,
                                            cpuif_info->cpuif_netdev->name,
                                            cpuif_info->cpuif_netdev->mtu);
                        return -EINVAL;
                    }
                    if (new_mtu < BF_KNET_MIN_FRAME_SIZE) {
                        printk(KERN_ERR DRV_NAME ": Host interface %s mtu size "
                                            "should not be less than %d",
                                            priv->hostif_knetdev->name,
                                            BF_KNET_MIN_FRAME_SIZE);
                        return -EINVAL;
                    }
                    printk(KERN_INFO DRV_NAME ": %s changing MTU from %d to %d\n",
					                           netdev->name, netdev->mtu, new_mtu);
                    netdev->mtu = new_mtu;
                    return 0;
                }
            }
        }
    }
    printk(KERN_ERR DRV_NAME ": %s not found in the list\n", netdev->name);
    return -EINVAL;
}

/* Operations for the bf_knet owned netdevices */
static const struct net_device_ops bf_knet_ndev_ops = {
    .ndo_start_xmit = _bf_knet_tx,
    .ndo_open = _bf_knet_open,
    .ndo_stop = _bf_knet_stop,
    .ndo_get_stats = _bf_knet_get_stats,
    .ndo_set_mac_address = eth_mac_addr,
    .ndo_change_mtu = _bf_knet_change_mtu,
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 8, 13)
    .ndo_change_carrier = kndev_change_carrier,
#endif
};

static void knet_get_drvinfo(struct net_device *dev,
                             struct ethtool_drvinfo *info)
{
	strscpy(info->driver, DRV_NAME, sizeof(info->driver));
	strscpy(info->version, DRV_VERSION, sizeof(info->version));
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 12, 0)
static int bf_knet_hostif_get_link_ksettings(struct net_device *netdev, struct ethtool_link_ksettings *cmd) {

	bf_knet_priv_t *priv = netdev_priv(netdev);

	memcpy(&priv->link_ksettings, cmd, sizeof(*cmd));
	return 0;
}

static int bf_knet_hostif_set_link_ksettings(struct net_device *dev,
                                             const struct ethtool_link_ksettings *cmd) {
	bf_knet_priv_t *priv = netdev_priv(dev);

	memcpy(&priv->link_ksettings, cmd, sizeof(*cmd));
	return 0;
}

#else
static int bf_knet_hostif_get_settings(struct net_device *netdev, struct ethtool_cmd *ecmd) {
	/* 1G */
	ethtool_cmd_speed_set(ecmd, SPEED_1000);
	ecmd->duplex =  DUPLEX_FULL;
	ecmd->autoneg = AUTONEG_DISABLE;
	ecmd->supported  = 0;
	ecmd->advertising = 0;
	return 0;
}
#endif

static const struct ethtool_ops bf_knet_ethtool_ops = {
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 12, 0)
	.get_link_ksettings = bf_knet_hostif_get_link_ksettings,
	.set_link_ksettings = bf_knet_hostif_set_link_ksettings,
#else
	.get_settings = bf_knet_hostif_get_settings,
#endif
	.get_drvinfo  = knet_get_drvinfo,
	.get_link     = ethtool_op_get_link,
	.get_ts_info  = ethtool_op_get_ts_info,
};

static struct net_device *bf_knet_kndev_create(const char *name)
{
	struct net_device *knetdev;
	bf_knet_priv_t *priv;

	knetdev = alloc_etherdev(sizeof(bf_knet_priv_t));

	if (knetdev == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to create knetdev - %s: "
			   "netdev allocation failed\n",
			   KBUILD_MODNAME, __func__, __LINE__, name);
		return NULL;
	}

	strncpy(knetdev->name, name, IFNAMSIZ - 1);

	knetdev->netdev_ops = &bf_knet_ndev_ops;
	knetdev->ethtool_ops = &bf_knet_ethtool_ops;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 9)
	knetdev->destructor = knetdev_destructor;
#else
	knetdev->needs_free_netdev = false;
	knetdev->priv_destructor = knetdev_destructor;
#endif
// eth_random_addr(knetdev->dev_addr);
#ifdef SET_MODULE_OWNER
	SET_MODULE_OWNER(knetdev);
#endif
	priv = netdev_priv(knetdev);
	priv->tx_action = NULL;

	/* We use register_netdevice instead of register_netdev because it
	doesn't try to acquire rtnl lock.
	RTNL lock is already held when IOCTL call is made, calling
	register_netdev causes a deadlock.
	https://groups.google.com/forum/#!topic/comp.os.linux.development.system/olhSxwNQy3E
	*/
	if (register_netdevice(knetdev)) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to create knetdev - %s: "
			   "netdev registration failed\n",
			   KBUILD_MODNAME, __func__, __LINE__, name);
		goto err_register;
	}

	return knetdev;

err_register:
	free_netdev(knetdev);
	return NULL;
}

static bf_cpuif_info_t *bf_knet_cpuif_id_lookup(uint64_t cpuif_id)
{
	bf_cpuif_info_t *cpuif_info = NULL;
	bf_cpuif_info_t *cpuif_info_temp = NULL;
	int found = 0;

	list_for_each_entry_safe(cpuif_info, cpuif_info_temp, &_device_list,
				 list)
	{
		if (cpuif_info->cpuif_id == cpuif_id) {
			found = 1;
			break;
		}
	}
	if (found == 1)
		return cpuif_info;
	else
		return NULL;
}

void bf_knet_hostif_kndev_add(bf_knet_msg_hostif_kndev_add_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv;
	struct net_device *knetdev;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add hostif_knetdev %s:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hostif_knetdev.name, msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	if (!dev_valid_name(msg->hostif_knetdev.name)) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add hostif_knetdev "
			   "-%s:netdev name is invalid\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hostif_knetdev.name);
		msg->hdr.status = BF_KNET_E_NAME_INVALID;
		return;
	}

	if ((knetdev = bf_knet_kndev_create(msg->hostif_knetdev.name)) ==
	    NULL) {
		msg->hdr.status = BF_KNET_E_RESOURCE;
		return;
	}

	priv = netdev_priv(knetdev);
	/* knetdev address is used as hostif_id */
	priv->id = (uintptr_t)knetdev;
	priv->cpuif_netdev = cpuif_info->cpuif_netdev;
	priv->hostif_knetdev = knetdev;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
  priv->hostif_knetdev->max_mtu =
          BF_KNET_MAX_JUMBO_FRAME_SIZE - (ETH_HLEN + ETH_FCS_LEN);
  priv->hostif_knetdev->min_mtu = BF_KNET_MIN_FRAME_SIZE;
#endif

	/* If hostif mtu size is greater than cpuif mtu size
           then bring up the hostif mtu as equal to cpuif mtu
        */
        if (priv->hostif_knetdev->mtu > priv->cpuif_netdev->mtu) {
            priv->hostif_knetdev->mtu = priv->cpuif_netdev->mtu;
        }

	/* Ideally this statement should be protected by a lock that gaurantees
	only one writer (list_add or list_del). However this code is being
	executed
	in context of IOCTL call which acquires RTNL lock and gaurantees only
	one
	writer for this critical section. Hence no lock primitive is used here
	*/
	list_add_tail_rcu(&priv->list, &(cpuif_info->hostif_kndev_list));

	msg->hdr.status = BF_KNET_E_NONE;
	msg->hostif_knetdev.knet_hostif_id = (uintptr_t)knetdev;
	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::Successfully added %s hostif_netdev to bf_knet, "
		   "hostif_id = %llu\n",
		   KBUILD_MODNAME, knetdev->name, priv->id);

	return;
}

void bf_knet_hostif_kndev_delete(bf_knet_msg_hostif_kndev_delete_t *msg)
{
	bf_cpuif_info_t *cpuif_info = NULL;
	bf_knet_rx_filter_info_t *filter_info = NULL;
	bf_knet_priv_t *priv = NULL;
	bf_knet_tx_action_t *kact;
	int found = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);
	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to delete hostif_knetdev:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		if (priv->id == msg->hdr.id.hostif_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || priv == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to delete hostif_knetdev:No "
			   "hostif_knetdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.hostif_id);
		msg->hdr.status = BF_KNET_E_HOSTIF_NOT_FOUND;
		return;
	}
	// TODO: Cross check pointer reassignment (is it truly safe??)
	list_for_each_entry_rcu(filter_info, &cpuif_info->bf_knet_rx_pf_list,
				list)
	{
		if (filter_info->hostif_knetdev == priv->hostif_knetdev) {
			knet_debug(KERN_INFO, &bf_knet->dev,
				   "%s::hostif_kndev_delete: Removing stale "
				   "reference to %s in filter %llu",
				   KBUILD_MODNAME, priv->hostif_knetdev->name,
				   filter_info->rx_filter.spec.filter_id);
			rcu_assign_pointer(filter_info->hostif_knetdev, NULL);
			synchronize_rcu();
		}
	}

	list_del_rcu(&priv->list);
	synchronize_rcu();

	if (priv->tx_action != NULL) {
		/* Free up memory allocated for tx_action */
		kact = priv->tx_action;
		rcu_assign_pointer(priv->tx_action, NULL);
		synchronize_rcu();
		if (kact->count > 0)
			kfree(kact->pkt_mutation);
		kfree(kact);
	}
	knet_debug(KERN_INFO, &bf_knet->dev, "%s::Unregistering %s\n",
		   KBUILD_MODNAME, priv->hostif_knetdev->name);
	unregister_netdevice(priv->hostif_knetdev);
	msg->hdr.status = BF_KNET_E_NONE;
	return;
}

void bf_knet_cpuif_ndev_add(bf_knet_msg_cpuif_ndev_add_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv;
	char kndev_name[IFNAMSIZ];
	struct net_device *knetdev = NULL;
	struct net_device *cpuif_netdev = NULL;
	int cpuif_netdev_len = 0;

	// validate the length of the cpu interface net device
	cpuif_netdev_len = strnlen(msg->cpuif_netdev_name, BF_KNET_NETDEV_NAME_MAX);
	if (cpuif_netdev_len <= 0 || cpuif_netdev_len >= BF_KNET_NETDEV_NAME_MAX) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add cpuif_netdev: string "
			   "length for cpu net device is invalid \n",
			   KBUILD_MODNAME, __func__, __LINE__);
        msg->hdr.status = BF_KNET_E_NAME_INVALID;
        goto err_long_name;
	}

	// Call to dev_get_by_name inc refcnt for cpuif_netdev. This reference
	// is released when this interface is removed from KNET
	cpuif_netdev = dev_get_by_name(&init_net, msg->cpuif_netdev_name);
	if (!cpuif_netdev) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add cpuif_netdev: No "
			   "netdev with %s name found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->cpuif_netdev_name);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		goto err_ndev_not_found;
	}

	if ((cpuif_netdev_len + strlen(BF_KNET_CPU_INTF_PREFIX)) >= IFNAMSIZ) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add cpuif_netdev: netdev "
			   "name for cpuif_knetdev -k_%s is too long\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->cpuif_netdev_name);
		msg->hdr.status = BF_KNET_E_NAME_INVALID;
		goto err_long_name;
	}

	memset(kndev_name, 0, IFNAMSIZ);
	strncpy(kndev_name, BF_KNET_CPU_INTF_PREFIX, IFNAMSIZ - 1);
	strncat(kndev_name, msg->cpuif_netdev_name,
                IFNAMSIZ - sizeof(BF_KNET_CPU_INTF_PREFIX));

	if (!dev_valid_name(kndev_name)) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add cpuif_knetdev "
			   "-%s:netdev name is invalid\n",
			   KBUILD_MODNAME, __func__, __LINE__, kndev_name);
		msg->hdr.status = BF_KNET_E_NAME_INVALID;
		goto err_invalid_name;
	}

	if ((cpuif_info = kzalloc(sizeof(bf_cpuif_info_t), GFP_KERNEL)) ==
	    NULL) {
		msg->hdr.status = BF_KNET_E_RESOURCE;
		goto err_resource;
	}

	cpuif_info->cpuif_netdev = cpuif_netdev;

	INIT_LIST_HEAD(&cpuif_info->bf_knet_rx_pf_list);
	INIT_LIST_HEAD(&cpuif_info->hostif_kndev_list);
	rwlock_init(&cpuif_info->cpuif_lock);

	/* Create the cpuif_knetdev corresponding to the cpuif_netdev */
	if ((knetdev = bf_knet_kndev_create(kndev_name)) == NULL) {
		msg->hdr.status = BF_KNET_E_RESOURCE;
		goto err_kndev_create;
	}
	cpuif_info->cpuif_knetdev = knetdev;

	priv = netdev_priv(knetdev);
	priv->cpuif_netdev = cpuif_netdev;
	/* cpuif_knetdev packet are sent transparently to corresponding
	cpuif_netdev */
	rcu_assign_pointer(priv->tx_action, NULL);
	// priv->action_active = false;

	/* knetdev address is used as cpuif_id */
	cpuif_info->cpuif_id = (uintptr_t)knetdev;

	/* Define field for protocol handler registration.
	We sniff packets for eth type 0x9000 (internal to BFN) on this
	cpuif_netdev */
	cpuif_info->device_proto.type = htons(0x9000);
	cpuif_info->device_proto.func = bf_knet_rcv;
	cpuif_info->device_proto.dev = cpuif_netdev;

	list_add_tail_rcu(&cpuif_info->list, &_device_list);

	msg->knet_cpuif_id = (uintptr_t)knetdev;
	memcpy(msg->cpuif_knetdev_name, kndev_name, strlen(kndev_name) + 1);
	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::Successfully added %s cpuif_netdev to bf_knet, "
		   "cpuif_id = %llu, registered for eth_type = %x\n",
		   KBUILD_MODNAME, msg->cpuif_netdev_name, cpuif_info->cpuif_id,
		   ntohs(cpuif_info->device_proto.type));

	dev_add_pack(&cpuif_info->device_proto);
	msg->hdr.status = BF_KNET_E_NONE;
	return;

err_kndev_create:
	kfree(cpuif_info);
err_resource:
err_invalid_name:
err_long_name:
	if (cpuif_netdev) {
		dev_put(cpuif_netdev);
	}
err_ndev_not_found:
	return;
}

void bf_knet_cpuif_ndev_delete(bf_knet_msg_cpuif_ndev_delete_t *msg)
{
	bf_cpuif_info_t *cpuif_info = NULL;
	bf_knet_rx_filter_info_t *filter_info = NULL;
	bf_knet_rx_filter_info_t *filter_next = NULL;
	bf_knet_tx_action_t *kact;
	bf_knet_priv_t *priv = NULL;
	bf_knet_priv_t *priv_temp = NULL;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);
	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to delete cpuif_netdev:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_del_rcu(&cpuif_info->list);
	synchronize_rcu();
	dev_remove_pack(&cpuif_info->device_proto);

	unregister_netdevice(cpuif_info->cpuif_knetdev);

	list_for_each_entry_safe(filter_info, filter_next,
				 &cpuif_info->bf_knet_rx_pf_list, list)
	{
		knet_debug(
		    KERN_INFO, &bf_knet->dev,
		    "cpuif_netdev_delete: Deleting filter with id %llu\n",
		    filter_info->rx_filter.spec.filter_id);
		list_del_rcu(&filter_info->list);
		synchronize_rcu();
		if (filter_info->rx_filter.action.count > 0)
			kfree(filter_info->rx_filter.action.pkt_mutation);
		kfree(filter_info);
	}

	list_for_each_entry_safe(priv, priv_temp,
				 &cpuif_info->hostif_kndev_list, list)
	{
		list_del_rcu(&priv->list);
		synchronize_rcu();
		if (priv->tx_action != NULL) {
			kact = priv->tx_action;
			rcu_assign_pointer(priv->tx_action, NULL);
			synchronize_rcu();
			if (kact->count > 0)
				kfree(kact->pkt_mutation);
			kfree(kact);
		}
		knet_debug(KERN_INFO, &bf_knet->dev,
			   "%s::cpuif_netdev delete: Unregistering %s\n",
			   KBUILD_MODNAME, priv->hostif_knetdev->name);
		unregister_netdevice(priv->hostif_knetdev);
	}

	// Return back netdev reference obtained when netdev was added to KNET
	if (cpuif_info->cpuif_netdev) {
		dev_put(cpuif_info->cpuif_netdev);
	}

	kfree(cpuif_info);
	msg->hdr.status = BF_KNET_E_NONE;
	return;
}

int cmp_mutation(const void *a, const void *b)
{
	bf_knet_packet_mutation_t *mutation1 = (bf_knet_packet_mutation_t *)a;
	bf_knet_packet_mutation_t *mutation2 = (bf_knet_packet_mutation_t *)b;

	/* Among actions with same offset, peform strip first */
	if (mutation1->offset == mutation2->offset) {
		if (mutation1->mutation_type == BF_KNET_RX_MUT_STRIP)
			return -1;
		else
			return 1;
	}
	return (mutation1->offset - mutation2->offset);
}

/* Arrange Strip actions first and insert actions later. Within each group sort
 * by offsets */
int cmp_mutationv2(const void *a, const void *b)
{
	bf_knet_packet_mutation_t *mutation1 = (bf_knet_packet_mutation_t *)a;
	bf_knet_packet_mutation_t *mutation2 = (bf_knet_packet_mutation_t *)b;

	if (mutation1->mutation_type == mutation2->mutation_type) {
		return (mutation1->offset - mutation2->offset);
	} else {
		if (mutation1->mutation_type == BF_KNET_RX_MUT_STRIP)
			return -1;
		else
			return 1;
	}
}

void bf_knet_rx_filter_add(bf_knet_msg_rx_filter_add_t *msg)
{

	bf_cpuif_info_t *cpuif_info;
	/* New node to be inserted into list */
	bf_knet_rx_filter_info_t *filter_info;
	/* Cursor for iterating over filter list */
	bf_knet_rx_filter_info_t *entry;
	/* Ptr to kernel rx action */
	bf_knet_rx_filter_action_t *kact;
	/* Ptr to user passed rx action */
	bf_knet_rx_filter_action_t *uact;
	/* Cursor for iterating over hostif_knetdev list */
	bf_knet_priv_t *priv;
	bf_knet_data_offset_t current_offset;
	/* Cursor for loop iteration */
	bf_knet_action_count_t i, j;
	/* Used to keep track of offset modification */
	int delta, delta_net, pkt_min_len, found;
	void __user *addr;

	delta = delta_net = pkt_min_len = found = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to add rx_filter:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	if ((filter_info = kzalloc(sizeof(bf_knet_rx_filter_info_t),
				   GFP_KERNEL)) == NULL) {
		msg->hdr.status = BF_KNET_E_RESOURCE;
		goto err_resource;
	}
	memcpy(&(filter_info->rx_filter.spec), &(msg->rx_filter.spec),
	       sizeof(bf_knet_rx_filter_spec_t));

	kact = &filter_info->rx_filter.action;
	uact = &msg->rx_filter.action;

	kact->dest_type = uact->dest_type;
	kact->dest_proto = uact->dest_proto;
	kact->count = uact->count;
	kact->knet_hostif_id = uact->knet_hostif_id;

	/* If dest_type is BF_FILTER_DESTINATION_HOSTIF,we lookup the hostif_id
	and
	save reference to its net_device struct,so as to avoid looking it up
	when
	this filter matches */
	if (kact->dest_type == BF_KNET_FILTER_DESTINATION_HOSTIF) {
		/* Lookup hostif_knetdev in the list of hostif and save
		 * reference to it */
		list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list,
					list)
		{
			if (priv->id == kact->knet_hostif_id) {
				found = 1;
				break;
			}
		}
		if (found == 0 || priv == NULL) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Rx Filter Add Failed: No "
				   "hostif_knetdev with id %llu found\n",
				   KBUILD_MODNAME, __func__, __LINE__,
				   msg->hdr.id.hostif_id);
			msg->hdr.status = BF_KNET_E_HOSTIF_NOT_FOUND;
			goto err_hostif_not_found;
		}
		filter_info->hostif_knetdev = priv->hostif_knetdev;
	}
	/* Copy packet mutation if any specified by the user */
	if (kact->count > 0) {
		kact->pkt_mutation =
		    kzalloc((kact->count) * sizeof(bf_knet_packet_mutation_t),
			    GFP_KERNEL);
		if (!kact->pkt_mutation) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Rx Filter Add failed: "
				   "Failed to allocate memory for Action "
				   "- Packet Mutation\n",
				   KBUILD_MODNAME, __func__, __LINE__);
			msg->hdr.status = BF_KNET_E_RESOURCE;
			goto err_enomem_pkt_mutation;
		}
		addr = (void __user *)(uact->pkt_mutation);
		if (copy_from_user(
			kact->pkt_mutation, (void *)(unsigned long)addr,
			(kact->count) * sizeof(bf_knet_packet_mutation_t))) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Rx Filter Add failed: Error "
				   "in copying Action - Packet Mutation\n",
				   KBUILD_MODNAME, __func__, __LINE__);
			msg->hdr.status = BF_KNET_E_MEM_ACCESS;
			goto err_copy_pkt_mutation;
		}
		/* Sorting of actions for maximum efficiency to be done by user
		 * space application */
		// sort(kact->pkt_mutation,kact->count,sizeof(bf_knet_packet_mutation_t),cmp_mutationv2,NULL);

		pkt_min_len = 0;
		/* Maximum size a packet can grow over its actual size. This
		value will be minimum if
		all strip actions are done first */
		delta_net = 0;
		for (i = 0; i < kact->count; i++) {
			pkt_min_len = kact->pkt_mutation[i].offset +
						  kact->pkt_mutation[i].len >
					      pkt_min_len
					  ? kact->pkt_mutation[i].offset +
						kact->pkt_mutation[i].len
					  : pkt_min_len;
			delta = (kact->pkt_mutation[i].mutation_type ==
				 BF_KNET_RX_MUT_INSERT)
				    ? kact->pkt_mutation[i].len
				    : -(kact->pkt_mutation[i].len);
			delta_net = (delta_net + delta > delta_net)
					? (delta_net + delta)
					: delta_net;
		}
		filter_info->pkt_min_len = pkt_min_len;
		filter_info->delta_net = delta_net;

		/* Offset and length validation to be done by user(Strip and/or
		 Insert regions do not overlap).
		 We simply readjust the offsets here assuming that offsets and
		 strip and insert lengths are correct */
		for (i = 0; i < kact->count; i++) {
			delta = (kact->pkt_mutation[i].mutation_type ==
				 BF_KNET_RX_MUT_INSERT)
				    ? kact->pkt_mutation[i].len
				    : -(kact->pkt_mutation[i].len);
			knet_debug(
			    KERN_INFO, &bf_knet->dev,
			    "%s::RX Filter Add: Pkt Mutation %d, delta =%d\n",
			    KBUILD_MODNAME, i, delta);
			/* If no mutation simply skip this action */
			if (delta == 0)
				continue;
			/* We adjust offset depending on strip/insert action */
			current_offset = kact->pkt_mutation[i].offset;
			for (j = i + 1; j < kact->count; j++) {
				/* If the offset is equal/less to current then,
				mutation doesn't change the offset */
				if (kact->pkt_mutation[j].offset >
				    current_offset) {
					/* Bail out, do not install filter as
					 * offset becomes invalid */
					if (kact->pkt_mutation[j].offset +
						delta >
					    0)
						kact->pkt_mutation[j].offset +=
						    delta;
					else {
						knet_debug(KERN_ERR,
							   &bf_knet->dev,
							   "%s:%s:%d::Aborting "
							   "installation of "
							   "filter, mutation "
							   "offset becomes "
							   "negative\n",
							   KBUILD_MODNAME,
							   __func__, __LINE__);
						knet_debug(
						    KERN_INFO, &bf_knet->dev,
						    "%s::Mutation[%d] "
						    "offset:%u "
						    "len:%u  Mutation[%d] "
						    "offset:%u len:%u\n",
						    KBUILD_MODNAME, i,
						    uact->pkt_mutation[j]
							.offset,
						    uact->pkt_mutation[i].len,
						    j, uact->pkt_mutation[j]
							   .offset,
						    uact->pkt_mutation[j].len);
						goto abort;
					}
				}
			}
		}
	}

	/* We add Rx filter sorted by their priority */
	list_for_each_entry_rcu(entry, &cpuif_info->bf_knet_rx_pf_list, list)
	{
		// printk(KERN_INFO "Scanning rx filters list\n");
		if (filter_info->rx_filter.spec.priority <
		    entry->rx_filter.spec.priority) {
			knet_debug(KERN_INFO, &bf_knet->dev,
				   "%s::Filter with priority %d being "
				   "inserted before filter %llu with "
				   "priority %d\n",
				   KBUILD_MODNAME,
				   filter_info->rx_filter.spec.priority,
				   entry->rx_filter.spec.filter_id,
				   entry->rx_filter.spec.priority);
			break;
		}
	}

	/* Filter is always added before the node that was found, or in case
	the entire list is traversed and no node is found with higher priority
	i.e. either list is empty or current filters have lower priorities, then
	the node is inserted at tail of list */
	list_add_tail_rcu(&filter_info->list, &((entry->list)));
	// list_add_rcu(&filter_info->list,((entry->list).prev));

	msg->hdr.status = BF_KNET_E_NONE;
	filter_info->rx_filter.spec.filter_id = (uintptr_t)filter_info;
	msg->rx_filter.spec.filter_id = filter_info->rx_filter.spec.filter_id;
	knet_debug(
	    KERN_INFO, &bf_knet->dev,
	    "%s::Successfully added rx_filter with priority %d, id = %llu\n",
	    KBUILD_MODNAME, msg->rx_filter.spec.priority,
	    msg->rx_filter.spec.filter_id);
	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::%d bytes headroom will be required for mutation "
		   "on this filter\n",
		   KBUILD_MODNAME, delta_net);
	return;
abort:
	msg->hdr.status = BF_KNET_E_PARAM;
err_copy_pkt_mutation:
	kfree(kact->pkt_mutation);
err_enomem_pkt_mutation:
err_hostif_not_found:
	kfree(filter_info);
err_resource:
	return;
}

void bf_knet_rx_filter_delete(bf_knet_msg_rx_filter_delete_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	int found = 0;
	bf_knet_rx_filter_info_t *filter_info = NULL;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to delete rx filter:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_for_each_entry_rcu(filter_info, &cpuif_info->bf_knet_rx_pf_list,
				list)
	{
		if (filter_info->rx_filter.spec.filter_id ==
		    msg->hdr.id.filter_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || filter_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to delete rx filter:No "
			   "filter with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.filter_id);
		msg->hdr.status = BF_KNET_E_RX_FILTER_NOT_FOUND;
		return;
	}
	list_del_rcu(&filter_info->list);
	synchronize_rcu();
	if (filter_info->rx_filter.action.count > 0)
		kfree(filter_info->rx_filter.action.pkt_mutation);
	kfree(filter_info);
	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::Successfully deleted filter with id %llu found\n",
		   KBUILD_MODNAME, msg->hdr.id.filter_id);
	msg->hdr.status = BF_KNET_E_NONE;
	return;
}

void bf_knet_tx_action_add(bf_knet_msg_tx_action_add_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv = NULL;
	bf_knet_tx_action_t *kact;
	bf_knet_tx_action_t *uact;
	bf_knet_data_offset_t current_offset;
	void __user *addr;
	int found, delta, delta_net, pkt_min_len;
	/* Loop iterators */
	int i, j = 0;
	found = delta = delta_net = pkt_min_len = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Add tx action:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		if (priv->id == msg->hdr.id.hostif_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || priv == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Add tx action:No "
			   "hostif_knetdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.hostif_id);
		msg->hdr.status = BF_KNET_E_HOSTIF_NOT_FOUND;
		return;
	}

	if ((kact = kzalloc(sizeof(bf_knet_tx_action_t), GFP_KERNEL)) == NULL) {
		knet_debug(
		    KERN_ERR, &bf_knet->dev,
		    "%s:%s:%d::Failed to allocate memory for Tx action\n",
		    KBUILD_MODNAME, __func__, __LINE__);
		msg->hdr.status = BF_KNET_E_RESOURCE;
		return;
	}

	uact = &msg->tx_action;
	kact->count = uact->count;

	/* Copy packet mutation if any specified by the user */
	if (kact->count > 0) {
		kact->pkt_mutation =
		    kzalloc((kact->count) * sizeof(bf_knet_packet_mutation_t),
			    GFP_KERNEL);
		if (!kact->pkt_mutation) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Failed to Add tx "
				   "action:Failed to allocate memory for "
				   "Action - Packet Mutation\n",
				   KBUILD_MODNAME, __func__, __LINE__);
			msg->hdr.status = BF_KNET_E_RESOURCE;
			goto err_enomem_pkt_mutation;
		}
		addr = (void __user *)(uact->pkt_mutation);
		if (copy_from_user(
			kact->pkt_mutation, (void *)(unsigned long)addr,
			(kact->count) * sizeof(bf_knet_packet_mutation_t))) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Failed to Add tx "
				   "action:Error in copying Action - "
				   "Packet Mutation\n",
				   KBUILD_MODNAME, __func__, __LINE__);
			msg->hdr.status = BF_KNET_E_MEM_ACCESS;
			goto err_copy_pkt_mutation;
		}
		// sort(kact->pkt_mutation,kact->count,sizeof(bf_knet_packet_mutation_t),cmp_mutationv2,NULL);
		pkt_min_len = 0;
		/* Maximum size a packet can grow over its actual size. This
		value will be minimum if
		all strip actions are done first */
		delta_net = 0;
		for (i = 0; i < kact->count; i++) {
			pkt_min_len = kact->pkt_mutation[i].offset +
						  kact->pkt_mutation[i].len >
					      pkt_min_len
					  ? kact->pkt_mutation[i].offset +
						kact->pkt_mutation[i].len
					  : pkt_min_len;
			delta = (kact->pkt_mutation[i].mutation_type ==
				 BF_KNET_RX_MUT_INSERT)
				    ? kact->pkt_mutation[i].len
				    : -(kact->pkt_mutation[i].len);
			delta_net = (delta_net + delta > delta_net)
					? (delta_net + delta)
					: delta_net;
		}
		priv->pkt_min_len = pkt_min_len;
		priv->delta_net = delta_net;

		/* Offset and length validation to be done by user(Strip and/or
		 Insert regions do not overlap).
		 We simply readjust the offsets here assuming that offsets and
		 strip and insert lengths are correct */
		for (i = 0; i < kact->count; i++) {
			delta = (kact->pkt_mutation[i].mutation_type ==
				 BF_KNET_RX_MUT_INSERT)
				    ? kact->pkt_mutation[i].len
				    : -(kact->pkt_mutation[i].len);
			knet_debug(
			    KERN_INFO, &bf_knet->dev,
			    "%s:TX Filter Add: Pkt Mutation %d, delta =%d\n",
			    KBUILD_MODNAME, i, delta);
			/* If no mutation simply skip this action */
			if (delta == 0)
				continue;
			/* We adjust offset depending on strip/insert action */
			current_offset = kact->pkt_mutation[i].offset;
			for (j = i + 1; j < kact->count; j++) {
				/* If the offset is equal to current then,
				mutation doesn't
				change the offset */
				if (kact->pkt_mutation[j].offset >
				    current_offset) {
					/* Bail out, do not install filter as
					 * offset becomes invalid */
					if (kact->pkt_mutation[j].offset +
						delta >
					    0)
						kact->pkt_mutation[j].offset +=
						    delta;
					else {
						knet_debug(
						    KERN_ERR, &bf_knet->dev,
						    "%s:%s:%d::Aborting "
						    "installation of Tx "
						    "action,Mutaiton offset "
						    "becomes negative\n",
						    KBUILD_MODNAME, __func__,
						    __LINE__);
						knet_debug(
						    KERN_INFO, &bf_knet->dev,
						    "%s::Mutation[%d] "
						    "offset:%u len:%u "
						    "Mutation[%d] offset:%u "
						    "len:%u\n",
						    KBUILD_MODNAME, i,
						    uact->pkt_mutation[i]
							.offset,
						    kact->pkt_mutation[i].len,
						    j, kact->pkt_mutation[j]
							   .offset,
						    kact->pkt_mutation[j].len);
						goto abort;
					}
				}
			}
		}
	}

	/* This should solve the atomicity problem. Pointer dereference
	at read side critical section
	should be via rcu_dereferene */
	rcu_assign_pointer(priv->tx_action, kact);
	synchronize_rcu();

	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::Tx action added to %s, %d bytes headroom  will be "
		   "required for mutation of packets sent from this "
		   "hostif_knetdev\n",
		   KBUILD_MODNAME, priv->hostif_knetdev->name, priv->delta_net);
	msg->hdr.status = BF_KNET_E_NONE;
	return;
abort:
	msg->hdr.status = BF_KNET_E_PARAM;
err_copy_pkt_mutation:
	kfree(kact->pkt_mutation);
err_enomem_pkt_mutation:
	kfree(kact);
	return;
}

void bf_knet_tx_action_delete(bf_knet_msg_tx_action_delete_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv = NULL;
	bf_knet_tx_action_t *kact;
	int found = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Delete tx action:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		if (priv->id == msg->hdr.id.hostif_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || priv == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Delete tx action :No "
			   "hostif_knetdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.hostif_id);
		msg->hdr.status = BF_KNET_E_HOSTIF_NOT_FOUND;
		return;
	}

	if (priv->tx_action == NULL) {
		knet_debug(KERN_INFO, &bf_knet->dev,
			   "%s::Tx action Delete :No action found for "
			   "hostif_knetdev %s\n",
			   KBUILD_MODNAME, priv->hostif_knetdev->name);
		msg->hdr.status = BF_KNET_E_NONE;
		return;
	}

	kact = priv->tx_action;
	rcu_assign_pointer(priv->tx_action, NULL);
	synchronize_rcu();
	priv->pkt_min_len = 0;
	priv->delta_net = 0;

	if (kact->count > 0)
		kfree(kact->pkt_mutation);
	kfree(kact);
	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::Tx action Delete successful for hostif id  %llu\n",
		   KBUILD_MODNAME, msg->hdr.id.hostif_id);
	msg->hdr.status = BF_KNET_E_NONE;
	return;
}

static inline bf_knet_count_t
bf_knet_get_cpuif_cnt(bf_knet_msg_obj_cnt_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_count_t count = 0;

	list_for_each_entry_rcu(cpuif_info, &_device_list, list) { count++; }
	msg->hdr.status = BF_KNET_E_NONE;
	return count;
}

static inline bf_knet_count_t
bf_knet_get_hostif_cnt(bf_knet_msg_obj_cnt_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv;
	bf_knet_count_t count = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve hostif_knetdev "
			   "Count:No cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return 0;
	}

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		count++;
	}

	msg->hdr.status = BF_KNET_E_NONE;
	return count;
}

static inline bf_knet_count_t
bf_knet_get_rx_filter_cnt(bf_knet_msg_obj_cnt_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_rx_filter_info_t *rx_filter;
	bf_knet_count_t count = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve Rx Filter "
			   "Count:No cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return 0;
	}

	list_for_each_entry_rcu(rx_filter, &cpuif_info->bf_knet_rx_pf_list,
				list)
	{
		count++;
	}

	msg->hdr.status = BF_KNET_E_NONE;
	return count;
}

static inline bf_knet_count_t
bf_knet_get_rx_mutation_cnt(bf_knet_msg_obj_cnt_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_rx_filter_info_t *filter_info;
	bf_knet_rx_filter_action_t *kact;
	int found = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve Rx Mutation "
			   "Count:No cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return 0;
	}

	list_for_each_entry_rcu(filter_info, &cpuif_info->bf_knet_rx_pf_list,
				list)
	{
		if (filter_info->rx_filter.spec.filter_id ==
		    msg->hdr.id.filter_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || filter_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve Rx Mutation "
			   "Count:No filter with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.filter_id);
		msg->hdr.status = BF_KNET_E_RX_FILTER_NOT_FOUND;
		return 0;
	}

	kact = &filter_info->rx_filter.action;

	msg->hdr.status = BF_KNET_E_NONE;
	return kact->count;
}

static inline bf_knet_count_t
bf_knet_get_tx_mutation_cnt(bf_knet_msg_obj_cnt_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv;
	int found = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve Tx Mutation "
			   "Count:No cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return 0;
	}

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		if (priv->id == msg->hdr.id.hostif_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || priv == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve Tx Mutation "
			   "Count:No hostif_knetdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.hostif_id);
		msg->hdr.status = BF_KNET_E_HOSTIF_NOT_FOUND;
		return 0;
	}

	msg->hdr.status = BF_KNET_E_NONE;
	if (!priv->tx_action)
		return 0;
	return priv->tx_action->count;
}

void bf_knet_obj_cnt_get(bf_knet_msg_obj_cnt_get_t *msg)
{

	switch (msg->type) {
	case BF_KNET_O_CPUIF:
		msg->obj_count = bf_knet_get_cpuif_cnt(msg);
		break;
	case BF_KNET_O_HOSTIF:
		msg->obj_count = bf_knet_get_hostif_cnt(msg);
		break;
	case BF_KNET_O_RX_FILTER:
		msg->obj_count = bf_knet_get_rx_filter_cnt(msg);
		break;
	case BF_KNET_O_RX_MUTATION:
		msg->obj_count = bf_knet_get_rx_mutation_cnt(msg);
		break;
	case BF_KNET_O_TX_MUTATION:
		msg->obj_count = bf_knet_get_tx_mutation_cnt(msg);
		break;
	default:
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::obj_cnt_get:Invalid obj type:%d\n",
			   KBUILD_MODNAME, __func__, __LINE__, msg->type);
		msg->hdr.status = BF_KNET_E_OBJ_TYPE;
		return;
	}
}

void bf_knet_rx_filter_list_get(bf_knet_msg_rx_filter_list_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_rx_filter_info_t *filter_info = NULL;
	bf_knet_count_t count = 0;
	bf_knet_count_t size = 0;
	void __user *addr;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve RX Filter "
			   "list:No cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	size = msg->size;
	if (!size) {
		msg->hdr.status = BF_KNET_E_NONE;
		return;
	}

	addr = (void __user *)(msg->filter_list);
	list_for_each_entry_rcu(filter_info, &cpuif_info->bf_knet_rx_pf_list,
				list)
	{
		if (count < size) {
			if (copy_to_user(addr,
					 &filter_info->rx_filter.spec.filter_id,
					 sizeof(bf_knet_filter_t))) {
				msg->hdr.status = BF_KNET_E_MEM_ACCESS;
				goto err_mem_access;
			} else {
				count++;
				addr += sizeof(bf_knet_filter_t);
			}
		}
	}

	msg->hdr.status = BF_KNET_E_NONE;
err_mem_access:
	msg->size = count;
	return;
}

void bf_knet_rx_filter_get(bf_knet_msg_rx_filter_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_rx_filter_info_t *filter_info = NULL;
	bf_knet_rx_filter_action_t *kact;
	bf_knet_rx_filter_action_t *uact;
	bf_knet_count_t count = 0;
	void __user *addr;
	int found = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve RX Filter:No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_for_each_entry_rcu(filter_info, &cpuif_info->bf_knet_rx_pf_list,
				list)
	{
		if (filter_info->rx_filter.spec.filter_id ==
		    msg->hdr.id.filter_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || filter_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to Retrieve RX Filter:No rx "
			   "filter with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.id.filter_id);
		msg->hdr.status = BF_KNET_E_RX_FILTER_NOT_FOUND;
		return;
	}

	memcpy(&(msg->rx_filter.spec), &(filter_info->rx_filter.spec),
	       sizeof(bf_knet_rx_filter_spec_t));

	kact = &filter_info->rx_filter.action;
	uact = &msg->rx_filter.action;

	uact->dest_type = kact->dest_type;
	uact->dest_proto = kact->dest_proto;
	uact->count = kact->count;
	if (uact->dest_type == BF_KNET_FILTER_DESTINATION_HOSTIF)
		uact->knet_hostif_id = kact->knet_hostif_id;
	else
		uact->knet_hostif_id = 0;

	/* Copy packet mutation if any, if user has allocated enough space*/
	count = kact->count < msg->mutation_count ? kact->count
						  : msg->mutation_count;
	if (count) {
		/* Should Ideally not happen */
		if (!kact->pkt_mutation) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "Rx Filter Get Failed: Failed to "
				   "Retrieve Action - Packet Mutation\n");
			msg->hdr.status = BF_KNET_E_PARAM;
			return;
		}
		addr = (void __user *)(uact->pkt_mutation);
		if (copy_to_user((void *)(unsigned long)addr,
				 kact->pkt_mutation,
				 (count) * sizeof(bf_knet_packet_mutation_t))) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Rx Filter Get failed: Error "
				   "in copying Action - Packet Mutation\n",
				   KBUILD_MODNAME, __func__, __LINE__);
			msg->hdr.status = BF_KNET_E_MEM_ACCESS;
			return;
		}
	}
	msg->hdr.status = BF_KNET_E_NONE;
	/* Return to user the number of mutations being copied */
	msg->mutation_count = count;
	return;
}

void bf_knet_tx_action_get(bf_knet_msg_tx_action_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv = NULL;
	bf_knet_tx_action_t *kact;
	void __user *addr;
	int found = 0;
	bf_knet_count_t count = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);

	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "Failed to Retrieve tx action:No cpuif_netdev "
			   "with id %llu found\n",
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		if (priv->id == msg->hdr.id.hostif_id) {
			found = 1;
			break;
		}
	}

	if (found == 0 || priv == NULL) {
		knet_debug(
		    KERN_ERR, &bf_knet->dev,
		    "Tx action Get :No hostif_knetdev with id %llu found\n",
		    msg->hdr.id.hostif_id);
		msg->hdr.status = BF_KNET_E_HOSTIF_NOT_FOUND;
		return;
	}

	kact = priv->tx_action;
	if (!kact) {
		msg->hdr.status = BF_KNET_E_TX_ACTION_NOT_FOUND;
		return;
	}
	msg->tx_action.count = kact->count;
	/* Copy packet mutation if any, if user has allocated enough space*/
	count = kact->count < msg->mutation_count ? kact->count
						  : msg->mutation_count;
	if (count) {
		/* Should Ideally not happen */
		if (!kact->pkt_mutation) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "Tx Action Get Failed: No Packet "
				   "Mutations found\n");
			msg->hdr.status = BF_KNET_E_PARAM;
			return;
		}
		addr = (void __user *)(msg->tx_action.pkt_mutation);
		if (copy_to_user((void *)(unsigned long)addr,
				 kact->pkt_mutation,
				 (count) * sizeof(bf_knet_packet_mutation_t))) {
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "Tx Action Get failed: Error in "
				   "copying Action - Packet Mutation\n");
			msg->hdr.status = BF_KNET_E_MEM_ACCESS;
			return;
		}
	}
	msg->hdr.status = BF_KNET_E_NONE;
	/* Return to user the number of mutations copied */
	msg->mutation_count = count;
	return;
}

void bf_knet_cpuif_list_get(bf_knet_msg_cpuif_list_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info = NULL;
	void __user *addr;
	bf_knet_count_t size, count = 0;
	size_t id_offset, name_offset;

	size = msg->size;
	if (!size) {
		msg->hdr.status = BF_KNET_E_NONE;
		return;
	}

	id_offset = offsetof(bf_knet_cpuif_list_t, id);
	name_offset = offsetof(bf_knet_cpuif_list_t, name);
	addr = (void __user *)(msg->cpuif_list);
	list_for_each_entry_rcu(cpuif_info, &_device_list, list)
	{
		if (count < size) {
			if (copy_to_user(
				(void *)(unsigned long)addr + id_offset,
				&cpuif_info->cpuif_id, sizeof(uint64_t))) {
				goto err_copy;
			}
			if (copy_to_user(
				(void *)(unsigned long)addr + name_offset,
				cpuif_info->cpuif_netdev->name,
				strlen(cpuif_info->cpuif_netdev->name) + 1)) {
				goto err_copy;
			}
			addr += sizeof(bf_knet_cpuif_list_t);
			count++;
		}
	}
	/* Return to user #entries in the list */
	msg->size = count;
	msg->hdr.status = BF_KNET_E_NONE;
	return;
err_copy:
	msg->hdr.status = BF_KNET_E_MEM_ACCESS;
	return;
}

void bf_knet_hostif_list_get(bf_knet_msg_hostif_list_get_t *msg)
{
	bf_cpuif_info_t *cpuif_info;
	bf_knet_priv_t *priv = NULL;
	void __user *addr;
	size_t id_offset, name_offset;
	bf_knet_count_t size, count = 0;

	cpuif_info = bf_knet_cpuif_id_lookup(msg->hdr.knet_cpuif_id);
	if (cpuif_info == NULL) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::Failed to retrieve hostif list: No "
			   "cpuif_netdev with id %llu found\n",
			   KBUILD_MODNAME, __func__, __LINE__,
			   msg->hdr.knet_cpuif_id);
		msg->hdr.status = BF_KNET_E_CPUIF_NOT_FOUND;
		return;
	}

	size = msg->size;
	if (!size) {
		msg->hdr.status = BF_KNET_E_NONE;
		return;
	}

	id_offset = offsetof(bf_knet_hostif_list_t, id);
	name_offset = offsetof(bf_knet_hostif_list_t, name);
	addr = (void __user *)(msg->hostif_list);

	list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
	{
		if (count < size) {
			if (copy_to_user((void *)(unsigned long)addr +
					     id_offset,
					 &priv->id, sizeof(uint64_t))) {
				goto err_copy;
			}
			if (copy_to_user(
				(void *)(unsigned long)addr + name_offset,
				priv->hostif_knetdev->name,
				strlen(priv->hostif_knetdev->name) + 1)) {
				goto err_copy;
			}
			addr += sizeof(bf_knet_hostif_list_t);
			count++;
		}
	}

	/* Return to user #entries in the list */
	msg->size = count;
	msg->hdr.status = BF_KNET_E_NONE;
	return;
err_copy:
	msg->hdr.status = BF_KNET_E_MEM_ACCESS;
	return;
}

void bf_knet_is_mod_init(bf_knet_msg_is_mod_init_t *msg)
{
	msg->hdr.status = BF_KNET_E_NONE;
	return;
}

int bf_knet_handle_cmd_req(bf_knet_msg_t *kmsg)
{

	int err = -EINVAL;

	/* This check ensures validates that the message corresponds
	to our device and not to any other device which shares the same
	IOCTL command */
	if (kmsg->hdr.len != sizeof(bf_knet_msg_t)) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::IOCTL message header length "
			   "verification failed\n",
			   KBUILD_MODNAME, __func__, __LINE__);
		return err;
	}

	switch (kmsg->hdr.type) {
	case BF_KNET_M_CPUIF_NDEV_ADD:
		bf_knet_cpuif_ndev_add(&kmsg->cpuif_ndev_add);
		break;
	case BF_KNET_M_CPUIF_NDEV_DELETE:
		bf_knet_cpuif_ndev_delete(&kmsg->cpuif_ndev_delete);
		break;
	case BF_KNET_M_HOSTIF_KNDEV_ADD:
		bf_knet_hostif_kndev_add(&kmsg->hostif_kndev_add);
		break;
	case BF_KNET_M_HOSTIF_KNDEV_DELETE:
		bf_knet_hostif_kndev_delete(&kmsg->hostif_kndev_delete);
		break;
	case BF_KNET_M_RX_FILTER_ADD:
		bf_knet_rx_filter_add(&kmsg->rx_filter_add);
		break;
	case BF_KNET_M_RX_FILTER_DELETE:
		bf_knet_rx_filter_delete(&kmsg->rx_filter_delete);
		break;
	case BF_KNET_M_TX_ACTION_ADD:
		bf_knet_tx_action_add(&kmsg->tx_action_add);
		break;
	case BF_KNET_M_TX_ACTION_DELETE:
		bf_knet_tx_action_delete(&kmsg->tx_action_delete);
		break;
	// case BF_KNET_M_KNET_INFO_GET:
	//	break;
	case BF_KNET_M_RX_FILTER_GET:
		bf_knet_rx_filter_get(&kmsg->rx_filter_get);
		break;
	case BF_KNET_M_OBJ_CNT_GET:
		bf_knet_obj_cnt_get(&kmsg->obj_cnt_get);
		break;
	case BF_KNET_M_RX_FILTER_LIST_GET:
		bf_knet_rx_filter_list_get(&kmsg->rx_filter_list_get);
		break;
	case BF_KNET_M_TX_ACTION_GET:
		bf_knet_tx_action_get(&kmsg->tx_action_get);
		break;
	case BF_KNET_M_CPUIF_LIST_GET:
		bf_knet_cpuif_list_get(&kmsg->cpuif_list_get);
		break;
	case BF_KNET_M_HOSTIF_LIST_GET:
		bf_knet_hostif_list_get(&kmsg->hostif_list_get);
		break;
	case BF_KNET_M_IS_MOD_INIT:
		bf_knet_is_mod_init(&kmsg->is_mod_init);
		break;
	default:
		kmsg->hdr.status = BF_KNET_E_MSG_TYPE;
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::IOCTL invalid command type\n",
			   KBUILD_MODNAME, __func__, __LINE__);
		return err;
	}
	err = 0;
	return err;
}

int _bf_knet_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	bf_knet_msg_t kmsg;
	void __user *addr = (void __user *)ifr->ifr_ifru.ifru_data;
	int err = -EFAULT;

	if (!atomic_read(&module_initialized)) {
		return -EFAULT;
	}
	if (addr != NULL) {
		switch (cmd) {
		case SIOCBFKNETCMD:
			if (copy_from_user(&kmsg, (void *)(unsigned long)addr,
					   sizeof(kmsg)))
				return -EFAULT;
			err = bf_knet_handle_cmd_req(&kmsg);
			break;
		default:
			knet_debug(KERN_ERR, &bf_knet->dev,
				   "%s:%s:%d::Invalid IOCTL %d\n",
				   KBUILD_MODNAME, __func__, __LINE__, cmd);
			err = -EINVAL;
			return err;
		}

		if (copy_to_user((void *)(unsigned long)addr, &kmsg,
				 sizeof(kmsg)))
			return -EFAULT;
	}
	return err;
}

/* Stub function , needed during bf_knet netdev allocation.
Dont need to do anything special here */
void bf_knet_setup(struct net_device *dev) {}

/* Operations for the bf_knet module */
static const struct net_device_ops bf_knet_ops = {
    .ndo_do_ioctl = _bf_knet_ioctl,
};

static void knet_set_msglevel(struct net_device *dev, u32 value)
{
	bf_knet_priv_t *knet = netdev_priv(bf_knet);
	knet->debug_level = value;
}

static u32 knet_get_msglevel(struct net_device *dev)
{
	bf_knet_priv_t *knet = netdev_priv(bf_knet);
	return knet->debug_level;
}


static const struct ethtool_ops knet_ethtool_ops = {
    .get_drvinfo = knet_get_drvinfo,
    .get_msglevel = knet_get_msglevel,
    .set_msglevel = knet_set_msglevel,
};

static void
bf_knet_change_host_mtu(struct net_device *netdev)
{
    bf_cpuif_info_t *cpuif_info;
    bf_knet_priv_t *priv;

    list_for_each_entry_rcu(cpuif_info, &_device_list, list)
    {
        if (!strcmp(cpuif_info->cpuif_netdev->name, netdev->name)) {
            list_for_each_entry_rcu(priv, &cpuif_info->hostif_kndev_list, list)
            {
                if (priv->hostif_knetdev->mtu > priv->cpuif_netdev->mtu) {
                    printk(KERN_INFO DRV_NAME ": %s changing mtu from %d to %d\n",
                                                 priv->hostif_knetdev->name,
                                                 priv->hostif_knetdev->mtu,
                                                 priv->cpuif_netdev->mtu);
                    priv->hostif_knetdev->mtu = priv->cpuif_netdev->mtu;
                }
            }
        }
    }
}

static int
bf_knet_device_event(struct notifier_block *this,
                                unsigned long event, void *ptr)
{
    struct net_device *netdev = netdev_notifier_info_to_dev(ptr);

    /* Only look at sockets that are using this specific device. */
    switch (event) {
    case NETDEV_CHANGEMTU:
        bf_knet_change_host_mtu(netdev);
        break;

    default:
        break;
  }
  return NOTIFY_DONE;
}

static struct notifier_block bf_knet_notifier = {
    .notifier_call = bf_knet_device_event,
};

static int __init bfknet_init(void)
{
	int err = 0;
	bf_knet_priv_t *knet = NULL;

	printk(KERN_INFO DRV_NAME ": %s - version %s\n", DRV_DESCRIPTION, DRV_VERSION);
	printk(KERN_INFO DRV_NAME ": %s\n", DRV_COPYRIGHT);
	printk(KERN_INFO DRV_NAME ": module loading ...");
	INIT_LIST_HEAD(&_device_list);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 17, 0)
	bf_knet =
	    alloc_netdev(sizeof(bf_knet_priv_t), "bf_knet", bf_knet_setup);
#else
	bf_knet = alloc_netdev(sizeof(bf_knet_priv_t), "bf_knet",
			       NET_NAME_UNKNOWN, bf_knet_setup);
#endif

	if (!bf_knet) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::module initialization failed - "
			   "netdev allocation failed\n",
			   KBUILD_MODNAME, __func__, __LINE__);
		return -ENOMEM;
	}

	bf_knet->netdev_ops = &bf_knet_ops;
	bf_knet->ethtool_ops = &knet_ethtool_ops;
	knet = netdev_priv(bf_knet);
	knet->debug_level = LOGLEVEL_ERR;

	err = register_netdev(bf_knet);
	if (err) {
		knet_debug(KERN_ERR, &bf_knet->dev,
			   "%s:%s:%d::module initialization failed - "
			   "netdev registration failed\n",
			   KBUILD_MODNAME, __func__, __LINE__);
		goto err_register;
	}

	register_netdevice_notifier(&bf_knet_notifier);
	printk(KERN_INFO DRV_NAME ": module loaded successfully");

	atomic_set(&module_initialized, 1);
	return 0;

err_register:
	free_netdev(bf_knet);
	return err;
}

static void __exit bfknet_exit(void)
{

	bf_cpuif_info_t *cpuif_info;
	bf_knet_rx_filter_info_t *filter_info = NULL;
	bf_knet_rx_filter_info_t *filter_next = NULL;
	bf_knet_tx_action_t *kact;
	bf_knet_priv_t *priv = NULL;
	bf_knet_priv_t *priv_temp = NULL;

	printk(KERN_INFO DRV_NAME ": module unloading ...");

	/* This lock ensures that no writers (ioctl) is currently
	under concurrent execution */
	rtnl_lock();
	atomic_set(&module_initialized, 0);
	/* We release the lock now as any new ioctls will find
	module as unitialized and return */
	rtnl_unlock();

	unregister_netdevice_notifier(&bf_knet_notifier);
	while (!list_empty(&_device_list)) {
		cpuif_info =
		    list_entry(_device_list.next, bf_cpuif_info_t, list);

		list_del_rcu(&cpuif_info->list);
		synchronize_rcu();
		knet_debug(KERN_INFO, &bf_knet->dev,
			   "%s::KNET context for "
			   "cpuif_netdev %s being deleted\n",
			   KBUILD_MODNAME, cpuif_info->cpuif_netdev->name);
		dev_remove_pack(&cpuif_info->device_proto);

		knet_debug(KERN_INFO, &bf_knet->dev, "%s::Unregistering %s\n",
			   KBUILD_MODNAME, cpuif_info->cpuif_netdev->name);
		unregister_netdev(cpuif_info->cpuif_knetdev);

		knet_debug(KERN_INFO, &bf_knet->dev,
			   "%s::Deleting all Rx packet "
			   "filters for cpuif_netdev %s\n",
			   KBUILD_MODNAME, cpuif_info->cpuif_netdev->name);
		list_for_each_entry_safe(filter_info, filter_next,
					 &cpuif_info->bf_knet_rx_pf_list, list)
		{
			list_del_rcu(&filter_info->list);
			synchronize_rcu();
			knet_debug(
			    KERN_INFO, &bf_knet->dev,
			    "%s:: Deleting filter with id %llu, priority %d\n",
			    KBUILD_MODNAME,
			    filter_info->rx_filter.spec.filter_id,
			    filter_info->rx_filter.spec.priority);
			if (filter_info->rx_filter.action.count > 0)
				kfree(
				    filter_info->rx_filter.action.pkt_mutation);
			kfree(filter_info);
		}

		knet_debug(KERN_INFO, &bf_knet->dev,
			   "%s::Deleting all "
			   "hostif_knetdevs for cpuif_netdev %s\n",
			   KBUILD_MODNAME, cpuif_info->cpuif_netdev->name);
		list_for_each_entry_safe(priv, priv_temp,
					 &cpuif_info->hostif_kndev_list, list)
		{
			list_del_rcu(&priv->list);
			synchronize_rcu();
			if (priv->tx_action != NULL) {
				/* Free up memory allocated for tx_action */
				kact = priv->tx_action;
				rcu_assign_pointer(priv->tx_action, NULL);
				synchronize_rcu();
				if (kact->count > 0)
					kfree(kact->pkt_mutation);
				kfree(kact);
			}
			knet_debug(KERN_INFO, &bf_knet->dev,
				   "%s::Unregistering %s\n", KBUILD_MODNAME,
				   priv->hostif_knetdev->name);
			unregister_netdev(priv->hostif_knetdev);
		}
		// Return back netdev reference obtained when netdev was added
		// to KNET
		if (cpuif_info->cpuif_netdev) {
			dev_put(cpuif_info->cpuif_netdev);
		}
		kfree(cpuif_info);
	}
	knet_debug(KERN_INFO, &bf_knet->dev,
		   "%s::KNET context successfully deleted\n", KBUILD_MODNAME);

	unregister_netdev(bf_knet);
	free_netdev(bf_knet);

	printk(KERN_INFO DRV_NAME ": module unloaded successfully");
}

module_init(bfknet_init);
module_exit(bfknet_exit);

module_param(debug, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Enable/Disable Debug prints");

MODULE_DESCRIPTION("Barefoot KNET Packet Filtering");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Barefoot Networks");
