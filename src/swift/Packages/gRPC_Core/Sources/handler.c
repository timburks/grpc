/*
 *
 * Copyright 2016, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "internal.h"
#include "shim.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void grpcshim_handler_destroy(grpcshim_handler *h) {
  grpc_completion_queue_shutdown(h->completion_queue);
  grpcshim_completion_queue_drain(h->completion_queue);
  grpc_completion_queue_destroy(h->completion_queue);
  grpc_metadata_array_destroy(&(h->request_metadata_recv));
  grpc_call_details_destroy(&(h->call_details));
  if (h->server_call) {
    grpc_call_destroy(h->server_call);
  }
  free(h);
}

const char *grpcshim_handler_host(grpcshim_handler *h) {
  return h->call_details.host;
}

const char *grpcshim_handler_method(grpcshim_handler *h) {
  return h->call_details.method;
}

const char *grpcshim_handler_call_peer(grpcshim_handler *h) {
  return grpc_call_get_peer(h->server_call);
}

grpcshim_call *grpcshim_handler_get_call(grpcshim_handler *h) {
  grpcshim_call *call = (grpcshim_call *) malloc(sizeof(grpcshim_call));
  memset(call, 0, sizeof(grpcshim_call));
  call->call = h->server_call;
  return call;
}

grpcshim_completion_queue *grpcshim_handler_get_completion_queue(grpcshim_handler *h) {
  return h->completion_queue;
}

grpc_completion_type grpcshim_handler_wait_for_request(grpcshim_handler *h,
                                                       grpcshim_metadata_array *metadata,
                                                       double timeout) {
  void *tag101 = grpcshim_create_tag(101);
  grpc_call_error error = grpc_server_request_call(h->server->server,
                                                   &(h->server_call),
                                                   &(h->call_details),
                                                   metadata,
                                                   h->completion_queue,
                                                   h->server->completion_queue,
                                                   tag101);
  assert(error == GRPC_CALL_OK);

  // wait for a request
  return grpcshim_completion_queue_get_next_event(h->server->completion_queue, timeout);
}




