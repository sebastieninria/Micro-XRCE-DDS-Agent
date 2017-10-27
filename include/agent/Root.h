// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROOT_H
#define _ROOT_H

#include <agent/client/ProxyClient.h>
#include <agent/XRCEFactory.h>
#include <agent/XRCEParser.h>
#include "MessageQueue.h"

#include <transport/ddsxrce_transport.h>

#include <thread>
#include <memory>
#include <map>

namespace eprosima{
namespace micrortps{

class Agent;
class CREATE_Payload;
class DELETE_RESOURCE_Payload;

Agent* root();

/**
 * Class XRCE Agent. Handle XRCE messages and distribute them to different ProxyClients. It implements XRCEListener interface
 * for receibe messages from a XRCEParser.
 */
class Agent : public XRCEListener
{
public:
    Agent();
    ~Agent() = default;

    /*
     * Initialize the Agent.
     */   
    void init(const std::string& device);
    
    void init(uint16_t in_port, uint16_t out_port);

    /*
     * Creates and stores a ProxyClient
     * @param client_key: ProxyClient unique key.
     * @param create_info: Create client payload containing all the creation information.
     * @return ResultStatus struct with the operation result info.
     */
    ResultStatus create_client(const MessageHeader& header, const CREATE_CLIENT_Payload& create_info);

    /*
     * Removes a previously stored ProxyClient
     * @param client_key: ProxyClient unique key.
     * @param delete_info: Delete payload containing all the deletion information.
     * @return ResultStatus struct with the operation result info.
     */
    ResultStatus delete_client(ClientKey client_key, const DELETE_RESOURCE_Payload& delete_info);

    /*
     * Starts Agent loop to listen messages. It parses and dispaches those XRCE messages to its owner.
     */
    void run();

    void stop();


    /*
     * Receives a creation message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param create_client_payload: Creation information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const CREATE_CLIENT_Payload& create_client_payload) override;

    /*
     * Receives a creation message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param create_payload: Creation information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const CREATE_Payload& create_payload) override;

    /*
     * Receives a deletion message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param delete_payload: Deletion information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const DELETE_RESOURCE_Payload& delete_payload) override;

    /*
     * Receives a Write message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param write_payload: Write information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const WRITE_DATA_Payload& write_payload)  override;

    /*
     * Receives a Read message.
     * @param header: Message header.
     * @param sub_header: Submessage header.
     * @param read_payload: Read information.
     */
    void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const READ_DATA_Payload& read_payload)   override;

    ProxyClient* get_client(ClientKey client_key);

    void add_reply(const Message& message);
    void add_reply(const MessageHeader& header, const RESOURCE_STATUS_Payload& status_reply);
    void add_reply(const MessageHeader& header, const DATA_Payload_Data& payload);
    void add_reply(const MessageHeader& header, const DATA_Payload_Sample& payload);
    void add_reply(const MessageHeader& header, const DATA_Payload_DataSeq& payload);
    void add_reply(const MessageHeader& header, const DATA_Payload_SampleSeq& payload);
    void add_reply(const MessageHeader& header, const DATA_Payload_PackedSamples& payload);

private:

    locator_id_t loc_id_;
    static const size_t buffer_len_ = 1024;
    octet out_buffer_[buffer_len_];
    octet in_buffer_[buffer_len_];
    locator_id_t loc_;
    std::mutex clientsmtx_;
    std::map<ClientKey, ProxyClient> clients_;
    std::map<ObjectId, ClientKey> client_ids_;

    std::unique_ptr<std::thread> response_thread_;
    std::atomic<bool> running_;

    struct ResponseControl
    {
        std::atomic<bool> running_;
        std::condition_variable condition_;
        std::mutex data_structure_mutex_;
        std::mutex condition_variable_mutex_;
    } response_control_;
    
    MessageQueue messages_;
    
    void reply();
    void abort_execution();
    void demo_create_client();
    void demo_delete_subscriber(char* test_buffer, size_t buffer_size);
    void demo_message_create(char* test_buffer, size_t buffer_size);
    void demo_message_read(char* test_buffer, size_t buffer_size);
    void demo_process_response(Message& message);
    void demo_message_subscriber(char* test_buffer, size_t buffer_size);
    void demo_message_publisher(char* test_buffer, size_t buffer_size);
    void demo_message_write(char * test_buffer, size_t buffer_size);
    
    
    void update_header(MessageHeader& header);
};

} // eprosima
} // micrortps

#endif //_ROOT_H
