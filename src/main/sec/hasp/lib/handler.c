#define LOG_MODULE "sec-hasp-handler"

#include <string.h>

#include "handler.h"

#include "util/log.h"

enum sec_hasp_handler_packet_type {
    SEC_HASP_HANDLER_PT_APIUID = 0x2774,
    SEC_HASP_HANDLER_PT_LOGIN = 0x2711,
    SEC_HASP_HANDLER_PT_LOGOUT = 0x2712,
    SEC_HASP_HANDLER_PT_GETINFO = 0x2714,
    SEC_HASP_HANDLER_PT_SCHANNEL = 0x2716,
    SEC_HASP_HANDLER_PT_ENCRYPT = 0x2724,
    SEC_HASP_HANDLER_PT_DECRYPT = 0x2725,
    SEC_HASP_HANDLER_PT_READ = 0x271A,
    SEC_HASP_HANDLER_PT_WRITE = 0x271B,
};

void sec_hasp_handler_init(const uint8_t* key_data, size_t len)
{

}

struct sec_hasp_handler_transaction* sec_hasp_handler_alloc_transaction(size_t max_payload_size)
{
    struct sec_hasp_handler_transaction* trans;

    trans = malloc(sizeof(struct sec_hasp_handler_transaction));
    trans->req_resp_max_size = sizeof(struct sec_hasp_handler_header) + max_payload_size;
    trans->req = malloc(trans->req_resp_max_size);
    trans->resp = malloc(trans->req_resp_max_size);

    return trans;
}

void sec_hasp_handler_free_transaction(struct sec_hasp_handler_transaction* transaction)
{
    free(transaction->req);
    free(transaction->resp);
    free(transaction);
}

void sec_hasp_handler_do_transaction(struct sec_hasp_handler_transaction* transaction)
{
    log_debug("Req: packet_size %d, head_value 0x%X, transaction_num %d, session_id 0x%X, packet_type 0x%X, "
        "unknown 0x%X, tail_value 0x%X", transaction->req->header.packet_size, transaction->req->header.head_value,
        transaction->req->header.transaction_num, transaction->req->header.session_id,
        transaction->req->header.packet_type, transaction->req->header.tail_value);

    switch (transaction->req->header.packet_type) {
        case SEC_HASP_HANDLER_PT_APIUID:
            log_debug("SEC_HASP_HANDLER_PT_APIUID");
            break;

        case SEC_HASP_HANDLER_PT_LOGIN:
            log_debug("SEC_HASP_HANDLER_PT_LOGIN");
            break;

        case SEC_HASP_HANDLER_PT_LOGOUT:
            log_debug("SEC_HASP_HANDLER_PT_LOGOUT");
            break;

        case SEC_HASP_HANDLER_PT_GETINFO:
            log_debug("SEC_HASP_HANDLER_PT_GETINFO");
            break;

        case SEC_HASP_HANDLER_PT_SCHANNEL:
            log_debug("SEC_HASP_HANDLER_PT_SCHANNEL");
            break;

        case SEC_HASP_HANDLER_PT_ENCRYPT:
            log_debug("SEC_HASP_HANDLER_PT_ENCRYPT");
            break;

        case SEC_HASP_HANDLER_PT_DECRYPT:
            log_debug("SEC_HASP_HANDLER_PT_DECRYPT");
            break;

        case SEC_HASP_HANDLER_PT_READ:
            log_debug("SEC_HASP_HANDLER_PT_READ");
            break;

        case SEC_HASP_HANDLER_PT_WRITE:
            log_debug("SEC_HASP_HANDLER_PT_WRITE");
            break;

        default:
            log_error("Invalid packet type: %X", transaction->req->header.packet_type);
            memset(&transaction->req->header, 0, sizeof(struct sec_hasp_handler_header));
            break;
    }
}

//static void sec_hasp_handler_get_client_id(struct sec_hasp_handler_transaction* transaction)
//{
//    transaction->resp->header.packet_size
//    transaction->resp->header.head_value
//    transaction->resp->header.transaction_num
//    transaction->resp->header.session_id
//    transaction->resp->header.packet_type
//    transaction->resp->header.tail_value
//
//        rp = request_packet.payload_object
//
//        client_id = HaspUtils.make_fake_handle_value()
//        client_id_response = HaspObject.HO_Client_ID_Response()
//        client_id_response.populate(HaspConst.HASP_STATUS_OK,client_id)
//
//        self.client_db[client_id] = {
//            "hasp_serial":0,
//            "sessions":{}
//        }
//
//        return client_id_response
//}

