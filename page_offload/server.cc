#include "common.h"
erpc::Rpc<erpc::CTransport> *rpc;

void req_handler(erpc::ReqHandle *req_handle, void *_context) {
  // checking request data
  const erpc::MsgBuffer *req_msgbuf = req_handle->get_req_msgbuf();
  uint16_t response_msg = 0xEFBE;
  for(int i = 0 ; i < kReqMsgSize; i += 2){
    if(req_msgbuf->buf_[i] != 0xDE && req_msgbuf->buf_[i+1] != 0xAD){
      response_msg = 0xBAD;
    }
  }

  // sending response
  auto &resp = req_handle->pre_resp_msgbuf_;
  rpc->resize_msg_buffer(&resp, kResMsgSize);
  memcpy(resp.buf_, &response_msg, kResMsgSize);
  rpc->enqueue_response(req_handle, &resp);
}

int main() {
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);

  // initializing eRPC endpoint via nexus
  erpc::Nexus nexus(server_uri);

  // binding request handler for req type
  nexus.register_req_func(kReqType, req_handler);

  // creating RPC object
  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr, 0);

  // ready to respnd to client requests for this duration
  rpc->run_event_loop(100000);
}
