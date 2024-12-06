#include <stdio.h>
#include "rpc.h"

static const std::string kServerHostname = "mem-node.parth251-224310.fardatalab-pg0.utah.cloudlab.us";
static const std::string kClientHostname = "comp-node.parth251-224310.fardatalab-pg0.utah.cloudlab.us";

static constexpr uint16_t kUDPPort = 31850;
static constexpr uint8_t kReqType = 2;
static constexpr size_t kReqMsgSize = 4096 * 25;
static constexpr size_t kResMsgSize = 2;
