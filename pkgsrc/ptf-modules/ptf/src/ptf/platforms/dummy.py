"""
A dummy platform that returns an empty port map. Cannot be used for any
meaningful dataplane testing (unless ports are added dynamically as part of the
test).
"""


def platform_config_update(config):
    """
    Update configuration for the local platform

    @param config The configuration dictionary to use/update
    """

    config["port_map"] = {}
