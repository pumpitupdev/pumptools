#ifndef SEC_HASP_CONST_H
#define SEC_HASP_CONST_H

#define SEC_HAS_CONST_HEADER_SZ 24

#define SEC_HAS_CONST_FILEID_RW = 0xfff4
#define SEC_HAS_CONST_FILEID_RO = 0xfff5

#define SEC_HAS_CONST_SCOPE_LM                                  \
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><haspscope><hasp " \
  "type=\"HASP-HL\"><license_manager ip=\"127.0.0.1\" /></hasp></haspscope>"
#define SEC_HAS_CONST_SCOPE_HANDLE \
  "<haspscope><session handle=\"%d\"/></haspscope>"

#define SEC_HAS_CONST_FORMAT_GETID                                          \
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><haspformat><hasp><attribute " \
  "name=\"id\" /></hasp></haspformat>"
#define SEC_HAS_CONST_FORMAT_GETSESSION \
  "<haspformat root=\"hasp_info\"><si_feature /></haspformat>"
#define SEC_HAS_CONST_FORMAT_GETKEYINFO \
  "<haspformat root=\"hasp_info\"><si_container /></haspformat>"

#define SEC_HAS_CONST_SPEC_FEATURE_ID \
  "<haspspec><feature id=\"%d\" /></haspspec>"

enum sec_hasp_const_status {
  SEC_HASP_CONST_STATUS_OK = 0,
  SEC_HASP_CONST_STATUS_MEM_RANGE = 1,
  SEC_HASP_CONST_STATUS_INV_FILEID = 10,
  SEC_HASP_CONST_STATUS_NO_TIME = 12,
  SEC_HASP_CONST_STATUS_INV_VCODE = 22,
  SEC_HASP_CONST_STATUS_INV_HND = 9,
  SEC_HASP_CONST_STATUS_TOO_SHORT = 8,
  SEC_HASP_CONST_STATUS_DEVICE_ERR = 43,
  SEC_HASP_CONST_STATUS_SCHAN_ERR = 46,
  SEC_HASP_CONST_STATUS_BROKEN_SESSION = 0x7F000027
};

enum sec_hasp_const_operation_id {
  /* Not Used - Admin or Get LMS Version? */
  SEC_HASP_CONST_OP_ID_ADMIN = 0,
  /* Not Used */
  SEC_HASP_CONST_OP_ID_ECHO = 1,
  SEC_HASP_CONST_OP_ID_LOGIN = 0x2711,
  SEC_HASP_CONST_OP_ID_LOGOUT = 0x2712,
  SEC_HASP_CONST_OP_ID_LOGINSCOPE = 0x2713,
  SEC_HASP_CONST_OP_ID_GETINFO = 0x2714,
  SEC_HASP_CONST_OP_ID_PARSE_SCOPE = 0x2715,
  SEC_HASP_CONST_OP_ID_SETUPSCHANNEL = 0x2716,
  SEC_HASP_CONST_OP_ID_ENCRYPT = 0x2724,
  SEC_HASP_CONST_OP_ID_DECRYPT = 0x2725,
  SEC_HASP_CONST_OP_ID_READ = 0x271A,
  SEC_HASP_CONST_OP_ID_WRITE = 0x271B,
  SEC_HASP_CONST_OP_ID_GETSIZE = 0x271C,
  SEC_HASP_CONST_OP_ID_GETRTC = 0x271D,
  SEC_HASP_CONST_OP_ID_GETAPIUID = 0x2774,
  /* Not Used */
  SEC_HASP_CONST_OP_ID_UPDATE = 0x2775,
  /* Not Used */
  SEC_HASP_CONST_OP_ID_DETACH = 0x2779,
};

enum sec_hasp_const_operational_object_id {
  SEC_HASP_CONST_OID_CLIENTID_REQ = 0x7F34,
  SEC_HASP_CONST_OID_CLIENTID_RESP = 0x7F35,
  SEC_HASP_CONST_OID_LOGIN_REQ = 0x61,
  SEC_HASP_CONST_OID_LOGIN_REP = 0x62,
  SEC_HASP_CONST_OID_LOGINSCOPE_REQ = 0x63,
  SEC_HASP_CONST_OID_LOGINSCOPE_REP = 0x64,
  SEC_HASP_CONST_OID_LOGOUT_REQ = 0x65,
  SEC_HASP_CONST_OID_LOGOUT_REP = 0x66,
  SEC_HASP_CONST_OID_INFO_REQ = 0x67,
  SEC_HASP_CONST_OID_INFO_REP = 0x68,
  SEC_HASP_CONST_OID_READ_REQ = 0x6A,
  SEC_HASP_CONST_OID_READ_REP = 0x6B,
  SEC_HASP_CONST_OID_WRITE_REQ = 0x6C,
  SEC_HASP_CONST_OID_WRITE_REP = 0x6D,
  SEC_HASP_CONST_OID_GETSIZE_REQ = 0x6E,
  SEC_HASP_CONST_OID_GETSIZE_REP = 0x6F,
  SEC_HASP_CONST_OID_GETRTC_REQ = 0x70,
  SEC_HASP_CONST_OID_GETRTC_REP = 0x71,
  SEC_HASP_CONST_OID_SCHANNEL_REQ = 0x72,
  SEC_HASP_CONST_OID_SCHANNEL_REP = 0x73,
  SEC_HASP_CONST_OID_CRYPT_REQ = 0x77,
  SEC_HASP_CONST_OID_CRYPT_REP = 0x78,
};

#endif
