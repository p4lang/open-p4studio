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

#include "gtest.h"
#include <model_core/file-logger.h>

namespace rmt_utests {

TEST(TestFileLogger, DISABLED_CreateFiles) {
  model_core::ModelLogger mlog(nullptr);
  mlog.set_console_logging(true);

  // No per-chip per-pipe logging active by default
  mlog.log(-1,-1," 1. Top-level log - no chip/no pipe - logs in Model file\n");
  mlog.log( 0,-1," 2. Top-level log - chip0 specified but ignored - logs in Model file\n");
  mlog.log( 1,-1," 3. Top-level log - chip1 specified but ignored - logs in Model file\n");

  mlog.set_per_chip_logging(true);
  // All per-chip logs should now appear in dedicated file

  mlog.log(-1,-1," 4. Top-level log - no chip/no pipe - logs in Model file\n");
  mlog.log( 0,-1," 5. Chip0 log - no pipe logging ever - but logs now in Chip0 file\n");
  mlog.log( 1,-1," 6. Chip1 log - no pipe logging ever - but logs now in Chip1 file\n");
  // ChipLogger created for chip0 and chip1 has flags that do *not* allow per-pipe logginf
  mlog.log( 0, 0," 7. Chip0 log - pipe0 specified but ignored - logs in Chip0 file\n");
  mlog.log( 1, 0," 8. Chip1 log - pipe0 specified but ignored - logs in Chip1 file\n");

  mlog.set_per_pipe_logging(true);
  // All *new* chip per-pipe logs should have per-pipe logging

  mlog.log(-1,-1," 9. Top-level log - no chip/no pipe - logs in Model file\n");
  mlog.log( 0, 0,"10. Chip0 log - pipe0 specified but ignored - logs in Chip0 file\n");
  mlog.log( 1, 0,"11. Chip1 log - pipe0 specified but ignored - logs in Chip1 file\n");

  mlog.log( 2,-1,"12. Chip2 log - no pipe specified - logs in Chip2 file\n");

  mlog.log( 2, 0,"13. Chip2 log - pipe0 specified - logs in Chip2/Pipe0 file\n");
  mlog.log( 2, 1,"14. Chip2 log - pipe1 specified - logs in Chip2/Pipe1 file\n");
  mlog.log( 2, 9,"15. Chip2 log - pipe9 specified - logs in Chip2/Pipe9 file\n");

  mlog.log(-1,-1,"16. Top-level log - no chip/no pipe - logs in Model file\n");
}

TEST(TestFileLogger, DISABLED_CreateFilesInLogDir) {
  model_core::ModelLogger mlog(nullptr);
  mlog.set_log_dir("./logs");

  // No per-chip per-pipe logging active by default
  mlog.log(-1,-1," 1. Top-level log - no chip/no pipe - logs in Model file\n");
  mlog.log( 0,-1," 2. Top-level log - chip0 specified but ignored - logs in Model file\n");
  mlog.log( 1,-1," 3. Top-level log - chip1 specified but ignored - logs in Model file\n");

  mlog.set_per_chip_logging(true);
  // All per-chip logs should now appear in dedicated file

  mlog.log(-1,-1," 4. Top-level log - no chip/no pipe - logs in Model file\n");
  mlog.log( 0,-1," 5. Chip0 log - no pipe logging ever - but logs now in Chip0 file\n");
  mlog.log( 1,-1," 6. Chip1 log - no pipe logging ever - but logs now in Chip1 file\n");
  // ChipLogger created for chip0 and chip1 has flags that do *not* allow per-pipe logginf
  mlog.log( 0, 0," 7. Chip0 log - pipe0 specified but ignored - logs in Chip0 file\n");
  mlog.log( 1, 0," 8. Chip1 log - pipe0 specified but ignored - logs in Chip1 file\n");

  mlog.set_per_pipe_logging(true);
  // All *new* chip per-pipe logs should have per-pipe logging

  mlog.log(-1,-1," 9. Top-level log - no chip/no pipe - logs in Model file\n");
  mlog.log( 0, 0,"10. Chip0 log - pipe0 specified but ignored - logs in Chip0 file\n");
  mlog.log( 1, 0,"11. Chip1 log - pipe0 specified but ignored - logs in Chip1 file\n");

  mlog.log( 2,-1,"12. Chip2 log - no pipe specified - logs in Chip2 file\n");

  mlog.log( 2, 0,"13. Chip2 log - pipe0 specified - logs in Chip2/Pipe0 file\n");
  mlog.log( 2, 1,"14. Chip2 log - pipe1 specified - logs in Chip2/Pipe1 file\n");
  mlog.log( 2, 9,"15. Chip2 log - pipe9 specified - logs in Chip2/Pipe9 file\n");

  mlog.log(-1,-1,"16. Top-level log - no chip/no pipe - logs in Model file\n");
}

}
