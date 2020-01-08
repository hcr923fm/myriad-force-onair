#include "parse_opts.h"

using namespace std;
namespace fs = boost::filesystem;

void parse_options(string config_file)
{
    fs::path conf_file_path(config_file);
    assert(fs::exists(conf_file_path));

    rapidjson::Document conf_doc;
    conf_doc.Parse(config_file.c_str());
    // for (rapidjson::Value::ConstMemberIterator itr = conf_doc.MemberBegin(); itr != conf_doc.MemberEnd(); ++itr)
    // {
    //     printf("%s\n", itr->name();
    // }

    if (!conf_doc.HasMember("MyriadHosts"))
    {
        printf("No Myriad Hosts found in config file!\n");
        exit(1);
    }

    if (!conf_doc["MyriadHosts"].IsArray())
    {
        printf("it's not an array\n");
    }

    auto hosts = conf_doc["MyriadHosts"].GetArray();
    for (size_t i = 0; i < hosts.Size(); i++)
    {
        printf("Got Myriad host from Config File: %s", hosts[i]["address"].GetString());
    }
}