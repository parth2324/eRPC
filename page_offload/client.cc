#include "common.h"
erpc::Rpc<erpc::CTransport> *rpc;
erpc::MsgBuffer req;
erpc::MsgBuffer resp;
size_t start;

void cont_func(void * _context, void * _tag) {
  double usec = erpc::to_usec(erpc::rdtsc() - start, rpc->get_freq_ghz());
  if(resp.buf_[0] == 0xBE && resp.buf_[1] == 0xEF) printf("Success.\n");
  else printf("Failure.\n");
  printf("Took %f usec for %ld pages.\n", usec, kReqMsgSize / 4096);
}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort);

  // initializing eRPC endpoint via nexus
  // client_uri, numa node, num_bg_threads for request processing
  erpc::Nexus nexus(client_uri, 0, 0);

  // nexus object, context object, unique rpc id, session management handler, physical port
  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler, 1);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort);

  // connect to remote endpoint
  int session_num = rpc->create_session(server_uri, 0);

  // spin till connected
  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  // allocate message buffers
  req = rpc->alloc_msg_buffer_or_die(kReqMsgSize);
  for(int i = 0 ; i < kReqMsgSize; i += 2){
    req.buf_[i] = 0xDE;
    req.buf_[i+1] = 0xAD;
  }
  resp = rpc->alloc_msg_buffer_or_die(kResMsgSize);

  // session_num, req_type, req_msgbuf, resp_msgbuf, cont_func, tag, cont_etid
  // req_type points to index in background nexus thread's req_func_arr_

  start = erpc::rdtsc();
  rpc->enqueue_request(session_num, kReqType, &req, &resp, cont_func, nullptr);
  rpc->run_event_loop(100);

  delete rpc;
}
