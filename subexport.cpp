#include "misc.h"
#include "speedtestutil.h"
#include "ini_reader.h"
#include "rapidjson_extra.h"
#include "webget.h"
#include "subexport.h"
#include "printout.h"

#include <iostream>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <yaml-cpp/yaml.h>

extern bool overwrite_original_rules;
extern string_array renames, emojis;
extern bool add_emoji, remove_old_emoji;
extern bool api_mode;

std::string vmessConstruct(std::string add, std::string port, std::string type, std::string id, std::string aid, std::string net, std::string cipher, std::string path, std::string host, std::string tls, int local_port)
{
    if(path == "")
        path = "/";
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("Type");
    writer.String("VMess");
    writer.Key("Remark");
    writer.String(std::string(add + ":" + port).data());
    writer.Key("Hostname");
    writer.String(add.data());
    writer.Key("Port");
    writer.Int(shortAssemble((unsigned short)stoi(port), (unsigned short)local_port));
    writer.Key("UserID");
    writer.String(id.data());
    writer.Key("AlterID");
    writer.Int(stoi(aid));
    writer.Key("EncryptMethod");
    writer.String(cipher.data());
    writer.Key("TransferProtocol");
    writer.String(net.data());
    if(net == "ws")
    {
        writer.Key("Host");
        writer.String(host.data());
        writer.Key("Path");
        writer.String(path.data());
    }
    else
    {
        if(net == "quic")
        {
            writer.Key("QUICSecure");
            writer.String(host.data());
            writer.Key("QUICSecret");
            writer.String(path.data());
        }
        writer.Key("FakeType");
        writer.String(type.data());
    }
    writer.Key("TLSSecure");
    writer.Bool(tls == "tls");
    writer.EndObject();
    return sb.GetString();
}

std::string ssrConstruct(std::string group, std::string remarks, std::string remarks_base64, std::string server, std::string port, std::string protocol, std::string method, std::string obfs, std::string password, std::string obfsparam, std::string protoparam, int local_port, bool libev)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("Type");
    writer.String("SSR");
    writer.Key("Remark");
    writer.String(remarks.data());
    writer.Key("Hostname");
    writer.String(server.data());
    writer.Key("Port");
    writer.Int(shortAssemble((unsigned short)stoi(port), (unsigned short)local_port));
    writer.Key("Password");
    writer.String(password.data());
    writer.Key("EncryptMethod");
    writer.String(method.data());
    writer.Key("Protocol");
    writer.String(protocol.data());
    writer.Key("ProtocolParam");
    writer.String(protoparam.data());
    writer.Key("OBFS");
    writer.String(obfs.data());
    writer.Key("OBFSParam");
    writer.String(obfsparam.data());
    writer.EndObject();
    return sb.GetString();
}

std::string ssConstruct(std::string server, std::string port, std::string password, std::string method, std::string plugin, std::string pluginopts, std::string remarks, int local_port, bool libev)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("Type");
    writer.String("SS");
    writer.Key("Remark");
    writer.String(remarks.data());
    writer.Key("Hostname");
    writer.String(server.data());
    writer.Key("Port");
    writer.Int(shortAssemble((unsigned short)stoi(port), (unsigned short)local_port));
    writer.Key("Password");
    writer.String(password.data());
    writer.Key("EncryptMethod");
    writer.String(method.data());
    writer.Key("Plugin");
    writer.String(plugin.data());
    writer.Key("PluginOption");
    writer.String(pluginopts.data());
    writer.EndObject();
    return sb.GetString();
}

std::string socksConstruct(std::string remarks, std::string server, std::string port, std::string username, std::string password)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("Type");
    writer.String("Socks5");
    writer.Key("Remark");
    writer.String(remarks.data());
    writer.Key("Hostname");
    writer.String(server.data());
    writer.Key("Port");
    writer.Int(stoi(port));
    writer.Key("Username");
    writer.String(username.data());
    writer.Key("Password");
    writer.String(password.data());
    writer.EndObject();
    return sb.GetString();
}

std::string vmessLinkConstruct(std::string remarks, std::string add, std::string port, std::string type, std::string id, std::string aid, std::string net, std::string path, std::string host, std::string tls)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("v");
    writer.String("2");
    writer.Key("ps");
    writer.String(remarks.data());
    writer.Key("add");
    writer.String(add.data());
    writer.Key("port");
    writer.Int(stoi(port));
    writer.Key("type");
    writer.String(type.data());
    writer.Key("id");
    writer.String(id.data());
    writer.Key("aid");
    writer.Int(stoi(aid));
    writer.Key("net");
    writer.String(net.data());
    writer.Key("path");
    writer.String(path.data());
    writer.Key("host");
    writer.String(host.data());
    writer.Key("tls");
    writer.String(tls.data());
    writer.EndObject();
    return sb.GetString();
}

std::string nodeRename(std::string remark)
{
    string_array vArray;

    for(std::string &x : renames)
    {
        vArray = split(x, "@");
        if(vArray.size() == 1)
        {
            remark = regReplace(remark, vArray[0], "");
        }
        else if(vArray.size() == 2)
        {
            remark = regReplace(remark, vArray[0], vArray[1]);
        }
        else
            continue;
    }
    return remark;
}

std::string removeEmoji(std::string remark)
{
    if(!remove_old_emoji)
        return remark;
    char emoji_id[2] = {(char)-16, (char)-97};
    while(true)
    {
        if(remark[0] == emoji_id[0] && remark[1] == emoji_id[1])
            remark = remark.substr(4);
        else
            break;
    }
    return remark;
}

std::string addEmoji(std::string remark)
{
    if(!add_emoji)
        return remark;
    string_array vArray;
    for(std::string &x : emojis)
    {
        vArray = split(x, ",");
        if(vArray.size() != 2)
            continue;
        if(regFind(remark, vArray[0]))
        {
            remark = vArray[1] + " " + remark;
            break;
        }
    }
    return remark;
}

void rulesetToClash(YAML::Node &base_rule, std::vector<ruleset_content> &ruleset_content_array)
{
    string_array allRules, vArray;
    std::string rule_group, rule_path, retrived_rules, strLine;
    std::stringstream strStrm;
    YAML::Node Rules;

    if(!overwrite_original_rules)
        Rules = base_rule["Rule"];

    for(ruleset_content &x : ruleset_content_array)
    {
        rule_group = x.rule_group;
        retrived_rules = x.rule_content;
        if(retrived_rules.find("[]") == 0)
        {
            allRules.emplace_back(retrived_rules.substr(2) + "," + rule_group);
            continue;
        }
        char delimiter = count(retrived_rules.begin(), retrived_rules.end(), '\n') < 1 ? '\r' : '\n';

        strStrm.clear();
        strStrm<<retrived_rules;
        while(getline(strStrm, strLine, delimiter))
        {
            strLine = replace_all_distinct(strLine, "\r", ""); //remove line break
            if(!strLine.size() || strLine.find("#") == 0 || strLine.find(";") == 0) //remove comments
                continue;
            if(strLine.find("USER-AGENT") == 0 || strLine.find("URL-REGEX") == 0 || strLine.find("PROCESS-NAME") == 0) //remove unsupported types
                continue;
            if(strLine.find("IP-CIDR") == 0)
                strLine = replace_all_distinct(strLine, ",no-resolve", "");
            else if(strLine.find("DOMAIN-SUFFIX") == 0)
                strLine = replace_all_distinct(strLine, ",force-remote-dns", "");
            strLine += "," + rule_group;
            allRules.emplace_back(strLine);
        }
    }

    for(std::string &x : allRules)
    {
        Rules.push_back(x);
    }
    base_rule["Rule"] = Rules;
}

void rulesetToSurge(INIReader &base_rule, string_array &ruleset_array, int surge_ver)
{
    string_array allRules, vArray;
    std::string rule_group, rule_path, retrived_rules, strLine;
    std::stringstream strStrm;

    base_rule.SetCurrentSection("Rule");

    if(overwrite_original_rules)
        base_rule.EraseSection();

    for(std::string &x : ruleset_array)
    {
        vArray = split(x, ",");
        if(vArray.size() != 2)
            continue;
        rule_group = trim(vArray[0]);
        rule_path = trim(vArray[1]);
        if(rule_path.find("[]") == 0)
        {
            strLine = rule_path.substr(2);
            if(strLine == "MATCH")
                strLine = "FINAL";
            allRules.emplace_back(strLine + "," + rule_group);
            continue;
        }
        else
        {
            if(fileExist(rule_path))
            {
                if(api_mode)
                    continue;
                retrived_rules = fileGet(rule_path, false);
                char delimiter = count(retrived_rules.begin(), retrived_rules.end(), '\n') < 1 ? '\r' : '\n';

                strStrm.clear();
                strStrm<<retrived_rules;
                while(getline(strStrm, strLine, delimiter))
                {
                    strLine = replace_all_distinct(strLine, "\r", ""); //remove line break
                    if(!strLine.size() || strLine.find("#") == 0 || strLine.find(";") == 0) //remove comments
                        continue;
                    if(surge_ver < 4 && strLine.find("IP-CIDR") == 0)
                        strLine = replace_all_distinct(strLine, ",no-resolve", "");
                    strLine += "," + rule_group;
                    allRules.emplace_back(strLine);
                }
            }
            else
            {
                allRules.emplace_back("RULE-SET," + rule_path + "," + rule_group);
            }
        }
    }

    for(std::string &x : allRules)
    {
        base_rule.Set("{NONAME}", x);
    }
}

std::string netchToClash(std::vector<nodeInfo> &nodes, std::string &baseConf, std::vector<ruleset_content> &ruleset_content_array, string_array &extra_proxy_group, bool clashR)
{
    YAML::Node yamlnode, proxies, singleproxy, singlegroup, original_groups;
    rapidjson::Document json;
    std::string type, remark, hostname, port, username, password, method;
    std::string plugin, pluginopts;
    std::string protocol, protoparam, obfs, obfsparam;
    std::string id, aid, transproto, faketype, host, path, quicsecure, quicsecret;
    std::vector<nodeInfo> nodelist;
    std::string group;
    bool tlssecure, replace_flag;
    string_array vArray, filtered_nodelist;

    try
    {
        yamlnode = YAML::Load(baseConf);
    }
    catch (std::exception &e)
    {
        return std::string();
    }

    for(nodeInfo &x : nodes)
    {
        singleproxy.reset();
        json.Parse(x.proxyStr.data());
        type = GetMember(json, "Type");
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = GetMember(json, "Port");
        username = GetMember(json, "Username");
        password = GetMember(json, "Password");
        method = GetMember(json, "EncryptMethod");

        if(type == "SS")
        {
            plugin = GetMember(json, "Plugin");
            pluginopts = replace_all_distinct(GetMember(json, "PluginOption"), ";", "&");
            singleproxy["type"] = "ss";
            singleproxy["cipher"] = method;
            if(plugin == "simple-obfs" || plugin == "obfs-local")
            {
                singleproxy["plugin"] = "obfs";
                singleproxy["plugin-opts"]["mode"] = UrlDecode(getUrlArg(pluginopts, "obfs"));
                singleproxy["plugin-opts"]["host"] = UrlDecode(getUrlArg(pluginopts, "obfs-host"));
            }
        }
        else if(type == "VMess")
        {
            id = GetMember(json, "UserID");
            aid = GetMember(json, "AlterID");
            transproto = GetMember(json, "TransferProtocol");
            host = GetMember(json, "Host");
            path = GetMember(json, "Path");
            tlssecure = GetMember(json, "TLSSecure") == "true";
            singleproxy["type"] = "vmess";
            singleproxy["uuid"] = id;
            singleproxy["alterId"] = stoi(aid);
            singleproxy["cipher"] = method;
            singleproxy["tls"] = tlssecure;
            if(transproto == "ws")
            {
                singleproxy["network"] = transproto;
                singleproxy["ws-path"] = path;
                singleproxy["ws-headers"]["Host"] = host;
            }
            else if(transproto == "kcp" || transproto == "h2" || transproto == "quic")
                continue;
        }
        else if(type == "SSR" && clashR)
        {
            protocol = GetMember(json, "Protocol");
            protoparam = GetMember(json, "ProtocolParam");
            obfs = GetMember(json, "OBFS");
            obfsparam = GetMember(json, "OBFSParam");
            singleproxy["type"] = "ssr";
            singleproxy["cipher"] = method;
            singleproxy["protocol"] = protocol;
            singleproxy["protocolparam"] = protoparam;
            singleproxy["obfs"] = obfs;
            singleproxy["obfsparam"] = obfsparam;
        }
        else if(type == "Socks5")
        {
            singleproxy["type"] = "socks5";
            singleproxy["username"] = username;
        }
        else if(type == "HTTP" || type == "HTTPS")
        {
            singleproxy["type"] = "http";
            singleproxy["username"] = username;
            singleproxy["tls"] = type == "HTTPS";
        }
        else
            continue;
        singleproxy["password"] = password;
        singleproxy["name"] = remark;
        singleproxy["server"] = hostname;
        singleproxy["port"] = (unsigned short)stoi(port);
        proxies.push_back(singleproxy);
        nodelist.emplace_back(x);
    }

    yamlnode["Proxy"] = proxies;
    std::string groupname;
    /*
    if(!overwrite_original_groups)
    {
        original_groups = yamlnode["Proxy Group"];
        for(unsigned int i = 0; i < original_groups.size(); i++)
        {
            groupname = original_groups[i]["name"].as<std::string>();
            if(groupname == "Proxy" || groupname == "PROXY")
            {
                original_groups[i]["name"] = "Proxy";
                original_groups[i]["proxies"] = vectorToNode(nodelist);
                break;
            }
        }
    }
    else
    {
        singlegroup["name"] = "Proxy";
        singlegroup["type"] = "select";
        singlegroup["proxies"] = vectorToNode(nodelist);
        original_groups.push_back(singlegroup);
    }
    */

    for(std::string &x : extra_proxy_group)
    {
        singlegroup.reset();
        eraseElements(filtered_nodelist);
        replace_flag = false;
        unsigned int rules_upper_bound = 0;

        vArray = split(x, "`");
        if(vArray.size() < 3)
            continue;

        if(vArray[1] == "select")
        {
            rules_upper_bound = vArray.size();
        }
        else if(vArray[1] == "url-test" || vArray[1] == "fallback" || vArray[1] == "load-balance")
        {
            if(vArray.size() < 5)
                continue;
            rules_upper_bound = vArray.size() - 2;
            singlegroup["url"] = vArray[vArray.size() - 2];
            singlegroup["interval"] = stoi(vArray[vArray.size() - 1]);
        }
        else
            continue;

        for(unsigned int i = 2; i < rules_upper_bound; i++)
        {
            if(vArray[i].find("[]") == 0)
            {
                filtered_nodelist.emplace_back(vArray[i].substr(2));
            }
            else if(vArray[i].find("!!GROUP=") == 0)
            {
                group = vArray[i].substr(8);
                for(nodeInfo &y : nodelist)
                {
                    if(regFind(y.group, group) && std::find(filtered_nodelist.begin(), filtered_nodelist.end(), y.remarks) == filtered_nodelist.end())
                        filtered_nodelist.emplace_back(y.remarks);
                }
            }
            else if(vArray[i].find("!!GROUPID=") == 0)
            {
                group = vArray[i].substr(10);
                for(nodeInfo &y : nodelist)
                {
                    if(y.groupID == stoi(group) && std::find(filtered_nodelist.begin(), filtered_nodelist.end(), y.remarks) == filtered_nodelist.end())
                        filtered_nodelist.emplace_back(y.remarks);
                }
            }
            else
            {
                for(nodeInfo &y : nodelist)
                {
                    if(regFind(y.remarks, vArray[i]) && std::find(filtered_nodelist.begin(), filtered_nodelist.end(), y.remarks) == filtered_nodelist.end())
                        filtered_nodelist.emplace_back(y.remarks);
                }
            }
        }

        if(!filtered_nodelist.size())
            filtered_nodelist.emplace_back("DIRECT");

        singlegroup["name"] = vArray[0];
        singlegroup["type"] = vArray[1];
        singlegroup["proxies"] = filtered_nodelist;

        for(unsigned int i = 0; i < original_groups.size(); i++)
        {
            if(original_groups[i]["name"].as<std::string>() == vArray[0])
            {
                original_groups[i] = singlegroup;
                replace_flag = true;
                break;
            }
        }
        if(!replace_flag)
            original_groups.push_back(singlegroup);
    }

    yamlnode["Proxy Group"] = original_groups;

    rulesetToClash(yamlnode, ruleset_content_array);

    return to_string(yamlnode);
}

std::string netchToSurge(std::vector<nodeInfo> &nodes, std::string &base_conf, string_array &ruleset_array, string_array &extra_proxy_group, int surge_ver)
{
    rapidjson::Document json;
    INIReader ini;
    std::string proxy;
    std::string type, remark, hostname, port, username, password, method;
    std::string plugin, pluginopts;
    std::string id, aid, transproto, faketype, host, path, quicsecure, quicsecret;
    std::string url, group;
    std::vector<nodeInfo> nodelist;
    bool tlssecure;
    string_array vArray, filtered_nodelist;

    ini.store_any_line = true;
    if(ini.Parse(base_conf) != 0)
        return std::string();

    ini.SetCurrentSection("Proxy");
    ini.EraseSection();
    ini.Set("DIRECT", "direct");
    for(nodeInfo &x : nodes)
    {
        json.Parse(x.proxyStr.data());
        type = GetMember(json, "Type");
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = std::__cxx11::to_string((unsigned short)stoi(GetMember(json, "Port")));
        username = GetMember(json, "Username");
        password = GetMember(json, "Password");
        method = GetMember(json, "EncryptMethod");
        proxy = "";

        if(type == "SS")
        {
            if(surge_ver >= 3)
            {
                plugin = GetMember(json, "Plugin");
                pluginopts = GetMember(json, "PluginOption");
                proxy = "ss," + hostname + "," + port + ",encrypt-method=" + method + ",password=" + password;
                if(plugin.size() && pluginopts.size())
                {
                    proxy += "," + replace_all_distinct(pluginopts, ";", ",");
                }
            }
            else
            {
                if(GetMember(json, "Plugin") == "")
                {
                    proxy = "custom,"  + hostname + "," + port + "," + method + "," + password + ",https://github.com/ConnersHua/SSEncrypt/raw/master/SSEncrypt.module";
                }
                else
                    continue;
            }

        }
        else if(type == "VMess")
        {
            if(surge_ver < 4)
                continue;
            id = GetMember(json, "UserID");
            aid = GetMember(json, "AlterID");
            transproto = GetMember(json, "TransferProtocol");
            host = GetMember(json, "Host");
            path = GetMember(json, "Path");
            tlssecure = GetMember(json, "TLSSecure") == "true";
            proxy = "vmess," + hostname + "," + port + "," + method + ",username=" + id + ",tls=" + (tlssecure ? "true" : "false");
            if(transproto == "ws")
            {
                proxy += ",ws=true,ws-path=" + path;
            }
            else if(transproto == "kcp" || transproto == "h2" || transproto == "quic")
                continue;
        }
        else if(type == "Socks5")
        {
            proxy = "socks5," + hostname + "," + port;
            if(username != "" && password != "")
                proxy += "," + username + "," + password;
        }
        else if(type == "HTTP" || type == "HTTPS")
        {
            proxy = "http," + hostname + "," + port;
            if(username != "" && password != "")
                proxy += "," + username + "," + password;
            proxy += std::string(",tls=") + (type == "HTTPS" ? "true" : "false");
        }
        else
            continue;
        ini.Set(remark, proxy);
        nodelist.emplace_back(x);
    }

    ini.SetCurrentSection("Proxy Group");
    for(std::string &x : extra_proxy_group)
    {
        eraseElements(filtered_nodelist);
        unsigned int rules_upper_bound = 0;
        url = "";
        proxy = "";

        vArray = split(x, "`");
        if(vArray.size() < 3)
            continue;

        if(vArray[1] == "select")
        {
            rules_upper_bound = vArray.size();
        }
        else if(vArray[1] == "url-test" || vArray[1] == "fallback" || vArray[1] == "load-balance")
        {
            if(vArray.size() < 5)
                continue;
            rules_upper_bound = vArray.size() - 2;
            url = vArray[vArray.size() - 2];
        }
        else
            continue;

        for(unsigned int i = 2; i < rules_upper_bound; i++)
        {
            if(vArray[i].find("[]") == 0)
            {
                filtered_nodelist.emplace_back(vArray[i].substr(2));
            }
            else if(vArray[i].find("!!GROUP=") == 0)
            {
                group = vArray[i].substr(8);
                for(nodeInfo &y : nodelist)
                {
                    if(regFind(y.group, group) && std::find(filtered_nodelist.begin(), filtered_nodelist.end(), y.remarks) == filtered_nodelist.end())
                        filtered_nodelist.emplace_back(y.remarks);
                }
            }
            else if(vArray[i].find("!!GROUPID=") == 0)
            {
                group = vArray[i].substr(10);
                for(nodeInfo &y : nodelist)
                {
                    if(y.groupID == stoi(group) && std::find(filtered_nodelist.begin(), filtered_nodelist.end(), y.remarks) == filtered_nodelist.end())
                        filtered_nodelist.emplace_back(y.remarks);
                }
            }
            else
            {
                for(nodeInfo &y : nodelist)
                {
                    if(regFind(y.remarks, vArray[i]) && std::find(filtered_nodelist.begin(), filtered_nodelist.end(), y.remarks) == filtered_nodelist.end())
                        filtered_nodelist.emplace_back(y.remarks);
                }
            }
        }

        if(!filtered_nodelist.size())
            filtered_nodelist.emplace_back("DIRECT");

        proxy = vArray[1];
        for(std::string &x : filtered_nodelist)
            proxy += "," + x;
        if(vArray[1] == "url-test" || vArray[1] == "fallback" || vArray[1] == "load-balance")
            proxy += ",url=" + url;

        ini.Set("{NONAME}", vArray[0] + " = " + proxy); //insert order
    }

    rulesetToSurge(ini, ruleset_array, surge_ver);

    return ini.ToString();
}

std::string netchToSS(std::vector<nodeInfo> &nodes)
{
    rapidjson::Document json;
    std::string remark, hostname, port, password, method;
    std::string plugin, pluginopts;
    std::string proxyStr, allLinks;
    for(nodeInfo &x : nodes)
    {
        json.Parse(x.proxyStr.data());
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = std::__cxx11::to_string((unsigned short)stoi(GetMember(json, "Port")));
        password = GetMember(json, "Password");
        method = GetMember(json, "EncryptMethod");
        plugin = GetMember(json, "Plugin");
        pluginopts = GetMember(json, "PluginOption");

        switch(x.linkType)
        {
        case SPEEDTEST_MESSAGE_FOUNDSS:
            proxyStr = "ss://" + urlsafe_base64_encode(method + ":" + password + "@" + hostname + ":" + port);
            if(plugin.size() & pluginopts.size())
            {
                proxyStr += "/?plugin=" + UrlEncode(plugin + ";" +pluginopts);
            }
            proxyStr += "#" + UrlEncode(remark);
            break;
        default:
            continue;
        }
        allLinks += proxyStr + "\n";
    }

    return base64_encode(allLinks);
}

std::string netchToSSR(std::vector<nodeInfo> &nodes)
{
    rapidjson::Document json;
    std::string remark, hostname, port, password, method;
    std::string protocol, protoparam, obfs, obfsparam;
    std::string proxyStr, allLinks;
    for(nodeInfo &x : nodes)
    {
        json.Parse(x.proxyStr.data());
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = std::__cxx11::to_string((unsigned short)stoi(GetMember(json, "Port")));
        password = GetMember(json, "Password");
        method = GetMember(json, "EncryptMethod");
        protocol = GetMember(json, "Protocol");
        protoparam = GetMember(json, "ProtocolParam");
        obfs = GetMember(json, "OBFS");
        obfsparam = GetMember(json, "OBFSParam");

        switch(x.linkType)
        {
        case SPEEDTEST_MESSAGE_FOUNDSSR:
            proxyStr = "ssr://" + urlsafe_base64_encode(hostname + ":" + port + ":" + protocol + ":" + method + ":" + obfs + ":" + urlsafe_base64_encode(password) \
                       + "/?group=" + urlsafe_base64_encode(x.group) + "&remarks=" + urlsafe_base64_encode(remark) \
                       + "&obfsparam=" + urlsafe_base64_encode(obfsparam) + "&protoparam=" + urlsafe_base64_encode(protoparam));
            break;
        default:
            continue;
        }
        allLinks += proxyStr + "\n";
    }

    return base64_encode(allLinks);
}

std::string netchToVMess(std::vector<nodeInfo> &nodes)
{
    rapidjson::Document json;
    std::string remark, hostname, port, method;
    std::string id, aid, transproto, faketype, host, path, quicsecure, quicsecret;
    std::string proxyStr, allLinks;
    bool tlssecure;
    for(nodeInfo &x : nodes)
    {
        json.Parse(x.proxyStr.data());
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = std::__cxx11::to_string((unsigned short)stoi(GetMember(json, "Port")));
        method = GetMember(json, "EncryptMethod");
        id = GetMember(json, "UserID");
        aid = GetMember(json, "AlterID");
        transproto = GetMember(json, "TransferProtocol");
        host = GetMember(json, "Host");
        path = GetMember(json, "Path");
        faketype = GetMember(json, "FakeType");
        tlssecure = GetMember(json, "TLSSecure") == "true";

        switch(x.linkType)
        {
        case SPEEDTEST_MESSAGE_FOUNDVMESS:
            proxyStr = "vmess://" + base64_encode(vmessLinkConstruct(remark, hostname, port, faketype, id, aid, transproto, path, host, tlssecure ? "tls" : ""));
            break;
        default:
            continue;
        }
        allLinks += proxyStr + "\n";
    }

    return base64_encode(allLinks);
}

std::string netchToQuan(std::vector<nodeInfo> &nodes)
{
    rapidjson::Document json;
    std::string remark, hostname, port, method, password;
    std::string plugin, pluginopts;
    std::string protocol, protoparam, obfs, obfsparam;
    std::string id, aid, transproto, faketype, host, path, quicsecure, quicsecret;
    std::string proxyStr, allLinks;
    bool tlssecure;
    for(nodeInfo &x : nodes)
    {
        json.Parse(x.proxyStr.data());
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = std::__cxx11::to_string((unsigned short)stoi(GetMember(json, "Port")));
        method = GetMember(json, "EncryptMethod");
        password = GetMember(json, "Password");

        switch(x.linkType)
        {
        case SPEEDTEST_MESSAGE_FOUNDVMESS:
            id = GetMember(json, "UserID");
            aid = GetMember(json, "AlterID");
            transproto = GetMember(json, "TransferProtocol");
            host = GetMember(json, "Host");
            path = GetMember(json, "Path");
            faketype = GetMember(json, "FakeType");
            tlssecure = GetMember(json, "TLSSecure") == "true";

            if(method == "auto")
                method = "chacha20-ietf-poly1305";
            proxyStr = remark + " = vmess, " + hostname + ", " + port + ", " + method + ", \"" + id + "\", group=" + x.group;
            if(transproto == "ws")
                proxyStr += ", obfs=ws, obfs-path=" + path + ", obfs-header=\"Host: " + host + "\"";
            if(tlssecure)
                proxyStr += ", over-tls=true, tls-host=" + host;
            proxyStr = "vmess://" + urlsafe_base64_encode(proxyStr);
            break;
        case SPEEDTEST_MESSAGE_FOUNDSSR:
            protocol = GetMember(json, "Protocol");
            protoparam = GetMember(json, "ProtocolParam");
            obfs = GetMember(json, "OBFS");
            obfsparam = GetMember(json, "OBFSParam");

            proxyStr = "ssr://" + urlsafe_base64_encode(hostname + ":" + port + ":" + protocol + ":" + method + ":" + obfs + ":" + urlsafe_base64_encode(password) \
                       + "/?group=" + urlsafe_base64_encode(x.group) + "&remarks=" + urlsafe_base64_encode(remark) \
                       + "&obfsparam=" + urlsafe_base64_encode(obfsparam) + "&protoparam=" + urlsafe_base64_encode(protoparam));
            break;
        case SPEEDTEST_MESSAGE_FOUNDSS:
            plugin = GetMember(json, "Plugin");
            pluginopts = GetMember(json, "PluginOption");
            proxyStr = "ss://" + urlsafe_base64_encode(method + ":" + password + "@" + hostname + ":" + port);
            if(plugin.size() & pluginopts.size())
            {
                proxyStr += "/?plugin=" + UrlEncode(plugin + ";" +pluginopts);
            }
            proxyStr += "&group=" + urlsafe_base64_encode(x.group) + "#" + UrlEncode(remark);
            break;
        default:
            continue;
        }
        allLinks += proxyStr + "\n";
    }

    return allLinks;
}

std::string netchToQuanX(std::vector<nodeInfo> &nodes)
{
    rapidjson::Document json;
    std::string remark, hostname, port, method;
    std::string password, plugin, pluginopts;
    std::string id, aid, transproto, host, path;
    std::string protocol, protoparam, obfs, obfsparam;
    std::string proxyStr, allLinks;
    for(nodeInfo &x : nodes)
    {
        json.Parse(x.proxyStr.data());
        remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
        hostname = GetMember(json, "Hostname");
        port = std::__cxx11::to_string((unsigned short)stoi(GetMember(json, "Port")));
        method = GetMember(json, "EncryptMethod");

        switch(x.linkType)
        {
        case SPEEDTEST_MESSAGE_FOUNDVMESS:
            id = GetMember(json, "UserID");
            transproto = GetMember(json, "TransferProtocol");
            host = GetMember(json, "Host");
            path = GetMember(json, "Path");
            if(method == "auto")
                method = "chacha20-ietf-poly1305";
            proxyStr = "vmess = " + hostname + ":" + port + ", method=" + method + ", password=" + id;
            if(transproto == "ws")
                proxyStr += ", obfs=ws, obfs-host=" + host + ", obfs-uri=" + path;
            break;
        case SPEEDTEST_MESSAGE_FOUNDSS:
            password = GetMember(json, "Password");
            plugin = GetMember(json, "Plugin");
            pluginopts = GetMember(json, "PluginOption");
            proxyStr = "shadowsocks = " + hostname + ":" + port + ", method=" + method + ", password=" + password;
            if(plugin.size() && pluginopts.size())
                proxyStr += ", " + replace_all_distinct(pluginopts, ";", ", ");
            break;
        case SPEEDTEST_MESSAGE_FOUNDSSR:
            password = GetMember(json, "Password");
            protocol = GetMember(json, "Protocol");
            protoparam = GetMember(json, "ProtocolParam");
            obfs = GetMember(json, "OBFS");
            obfsparam = GetMember(json, "OBFSParam");
            proxyStr = "shadowsocks = " + hostname + ":" + port + ", method=" + method + ", password=" + password + ", ssr-protocol=" + protocol;
            if(protoparam.size())
                proxyStr += ", ssr-protocol-param=" + protoparam;
            proxyStr += ", obfs=" + obfs;
            if(obfsparam.size())
                proxyStr += ", obfs-host=" + obfsparam;
            break;
        default:
            continue;
        }
        proxyStr += ", tag=" + remark;
        allLinks += proxyStr + "\n";
    }

    return allLinks;
}

std::string netchToSSD(std::vector<nodeInfo> &nodes, std::string &group)
{
    rapidjson::Document json;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    std::string remark, hostname, password, method;
    std::string plugin, pluginopts;
    std::string proxyStr, allLinks;
    int port, index = 0;
    if(!group.size())
        group = "SSD";

    writer.StartObject();
    writer.Key("airport");
    writer.String(group.data());
    writer.Key("port");
    writer.Int(1);
    writer.Key("encryption");
    writer.String("aes-128-gcm");
    writer.Key("password");
    writer.String("password");
    writer.Key("servers");
    writer.StartArray();

    for(nodeInfo &x : nodes)
    {
        switch(x.linkType)
        {
        case SPEEDTEST_MESSAGE_FOUNDSS:
            json.Parse(x.proxyStr.data());
            remark = x.remarks = addEmoji(trim(removeEmoji(nodeRename(x.remarks))));
            hostname = GetMember(json, "Hostname");
            port = (unsigned short)stoi(GetMember(json, "Port"));
            password = GetMember(json, "Password");
            method = GetMember(json, "EncryptMethod");
            plugin = GetMember(json, "Plugin");
            pluginopts = GetMember(json, "PluginOption");
            writer.StartObject();
            writer.Key("server");
            writer.String(hostname.data());
            writer.Key("port");
            writer.Int(port);
            writer.Key("encryption");
            writer.String(method.data());
            writer.Key("password");
            writer.String(password.data());
            writer.Key("plugin");
            writer.String(plugin.data());
            writer.Key("plugin_options");
            writer.String(pluginopts.data());
            writer.Key("remarks");
            writer.String(remark.data());
            writer.Key("id");
            writer.Int(index);
            writer.EndObject();
            break;
        default:
            continue;
        }
        index++;
    }
    writer.EndArray();
    writer.EndObject();
    return "ssd://" + base64_encode(sb.GetString());
}

std::string buildGistData(std::string name, std::string content)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.StartObject();
    writer.Key("description");
    writer.String("subconverter");
    writer.Key("public");
    writer.Bool(false);
    writer.Key("files");
    writer.StartObject();
    writer.Key(name.data());
    writer.StartObject();
    writer.Key("content");
    writer.String(content.data());
    writer.EndObject();
    writer.EndObject();
    writer.EndObject();
    return sb.GetString();
}

int uploadGist(std::string path, std::string content, bool writeManageURL)
{
    INIReader ini;
    rapidjson::Document json;
    std::string token, id, username, retData, url;
    int retVal = 0;

    if(!fileExist("gistconf.ini"))
    {
        std::cerr<<"gistconf.ini not found. Skipping...\n";
        return -1;
    }

    ini.ParseFile("gistconf.ini");
    if(ini.EnterSection("common") == 0)
    {
        token = ini.Get("token");
        if(!token.size())
        {
            std::cerr<<"No token is provided. Skipping...\n";
            return -1;
        }
        id = ini.Get("id");
        username = ini.Get("username");
        if(!id.size())
        {
            std::cerr<<"No gist id is provided. Creating new gist...\n";
            retVal = curlPost("https://api.github.com/gists", buildGistData(path, content), "", token, &retData);
            if(retVal != 201)
            {
                std::cerr<<"Create new Gist failed! Return data:\n"<<retData<<"\n";
                return -1;
            }
        }
        else
        {
            url = ini.Get(path, "url");
            if(!url.size())
                url = "https://gist.githubusercontent.com/" + username + "/" + id + "/raw/" + path;
            std::cerr<<"Gist id provided. Modifying gist...\n";
            if(writeManageURL)
                content = "#!MANAGED-CONFIG "+ url + "\n" + content;
            retVal = curlPatch("https://api.github.com/gists/" + id, buildGistData(path, content), "", token, &retData);
            if(retVal != 200)
            {
                std::cerr<<"Modify gist failed! Return data:\n"<<retData<<"\n";
                return -1;
            }
        }
        json.Parse(retData.data());
        GetMember(json, "id", id);
        if(json.HasMember("owner"))
            GetMember(json["owner"], "login", username);
        std::cerr<<"Writing to Gist success!\nPath: "<<path<<"\nRaw URL: "<<url<<"\nGist owner: "<<username<<"\n";
    }
    else
    {
        std::cerr<<"gistconf.ini has incorrect format. Skipping...\n";
        return -1;
    }
    ini.EraseSection();
    ini.Set("token", token);
    ini.Set("id", id);
    ini.Set("username", username);

    ini.SetCurrentSection(path);
    ini.EraseSection();
    ini.Set("url", url);

    ini.ToFile("gistconf.ini");
    return 0;
}
