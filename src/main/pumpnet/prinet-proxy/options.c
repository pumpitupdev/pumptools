#include "pumpnet/prinet-proxy/options.h"

#include "util/options.h"

#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_SOURCE_ADDR "proxy.source.addr"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_SOURCE_PORT "proxy.source.port"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_SERVER "proxy.pumpnet.server"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_GAME "proxy.pumpnet.game"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_MACHINE_ID "proxy.pumpnet.machine_id"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_VERBOSE_LOG_OUTPUT "proxy.pumpnet.verbose_log_output"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_CERT_DIR_PATH "proxy.pumpnet.cert_dir_path"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PATCH_UTIL_LOG_FILE "util.log.file"
#define PUMPNET_PRINET_PROXY_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL "util.log.level"

const struct util_options_def _options_def[] = {
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_SOURCE_ADDR,
        .description = "IPV4 of the source address to listen to for incoming connections, e.g. 0.0.0.0",
        .param = 's',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "0.0.0.0",
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_SOURCE_PORT,
        .description = "Port of the source to listen to for incoming connections, e.g. 1234",
        .param = 'p',
        .type = UTIL_OPTIONS_TYPE_INT,
        // stock port used on prime
        .default_value.i = 27370,
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_SERVER,
        .description = "Address (IPV4 or URL) and port of pumpnet server",
        .param = 'n',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "127.0.0.1:8080",
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_MACHINE_ID,
        .description = "Machine id for pumpnet",
        .param = 'm',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "0102030405060708",
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_GAME,
        .description = "Game ID required for server endpoint to talk to, e.g. prime (27), prime2 (28), xx(29)",
        .param = 'g',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = ASSET_GAME_VERSION_PRIME,
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_VERBOSE_LOG_OUTPUT,
        .description = "Enable verbose log output for pumpnet related things, e.g. logging network traffic",
        .param = 'v',
        .type = UTIL_OPTIONS_TYPE_BOOL,
        .default_value.b = false,
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_CERT_DIR_PATH,
        .description = "Path to a folder containing the client key, certificate and CA bundle to enable https communication",
        .param = 'c',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = NULL,
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PATCH_UTIL_LOG_FILE,
        .description = "Print the log output to the specified file",
        .param = 'o',
        .type = UTIL_OPTIONS_TYPE_STR,
        .default_value.str = "pri-crypt-proxy.log",
    },
    {
        .name = PUMPNET_PRINET_PROXY_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL,
        .description = "Set the log level (0-4)",
        .param = 'l',
        .type = UTIL_OPTIONS_TYPE_INT,
        .default_value.i = LOG_LEVEL_DEBUG,
    },
};

const struct util_options_defs _options_defs = {
    .usage_header =
        "Pumptools prinet-proxy for Pump It Up: Prime, Prime2 and XX, build " __DATE__ " " __TIME__ ", gitrev " STRINGIFY(GITREV) "\n"
        "Usage: ./pumpnet-prinet-proxy --options pumpnet-prinet-proxy.conf",
    .usage_param = 'h',
    .defs = _options_def,
    .ndefs = lengthof(_options_def)
};

bool pumpnet_prinet_proxy_options_init(int argc, char** argv, struct pumpnet_prinet_proxy_options* options)
{
    log_assert(argv);
    log_assert(options);

    struct util_options_opts* options_opt;

    util_options_init(argc, argv);
    options_opt = util_options_get(&_options_defs);

    if (!options_opt) {
        return false;
    }

    options->proxy.source.addr = util_options_get_str(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_SOURCE_ADDR);
    options->proxy.source.port = util_options_get_int(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_SOURCE_PORT);
    options->proxy.pumpnet.server = util_options_get_str(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_SERVER);
    options->proxy.pumpnet.machine_id = strtoull(
        util_options_get_str(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_MACHINE_ID),
        NULL,
        16);
    options->proxy.pumpnet.game = util_options_get_int(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_GAME);
    options->proxy.pumpnet.verbose_log_output =
        util_options_get_bool(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_VERBOSE_LOG_OUTPUT);
    options->proxy.pumpnet.cert_dir_path =
        util_options_get_str(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PROXY_PUMPNET_CERT_DIR_PATH);
    options->log.file = util_options_get_str(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PATCH_UTIL_LOG_FILE);
    options->log.level =
        (enum util_log_level) util_options_get_int(options_opt, PUMPNET_PRINET_PROXY_OPTIONS_STR_PATCH_UTIL_LOG_LEVEL);

    return true;
}