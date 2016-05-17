/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */


#include "src/collection/backend/in_memory-per_process.h"

#ifdef __cplusplus
#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#endif

#include "modsecurity/collection/variable.h"
#include "src/utils.h"

namespace modsecurity {
namespace collection {
namespace backend {


InMemoryPerProcess::InMemoryPerProcess() {
    this->reserve(1000);
}

InMemoryPerProcess::~InMemoryPerProcess() {
    this->clear();
}

void InMemoryPerProcess::store(std::string key, std::string value) {
    this->emplace(key, value);
}


bool InMemoryPerProcess::storeOrUpdateFirst(const std::string &key,
    const std::string &value) {
    if (updateFirst(key, value) == false) {
        store(key, value);
    }
    return true;
}


bool InMemoryPerProcess::updateFirst(const std::string &key,
    const std::string &value) {
    auto range = this->equal_range(key);

    for (auto it = range.first; it != range.second; ++it) {
        it->second = value;
        return true;
    }
    return false;
}


void InMemoryPerProcess::del(const std::string& key) {
    this->erase(key);
}


void InMemoryPerProcess::resolveSingleMatch(const std::string& var,
    std::vector<const Variable *> *l) {
    auto range = this->equal_range(var);

    for (auto it = range.first; it != range.second; ++it) {
        l->push_back(new Variable(var, it->second));
    }
}


void InMemoryPerProcess::resolveMultiMatches(const std::string& var,
    std::vector<const Variable *> *l) {
    size_t keySize = var.size();
    l->reserve(15);

    auto range = this->equal_range(var);

    for (auto it = range.first; it != range.second; ++it) {
        l->insert(l->begin(), new Variable(var, it->second));
    }

    for (const auto& x : *this) {
        if (x.first.size() <= keySize + 1) {
            continue;
        }
        if (x.first.at(keySize) != ':') {
            continue;
        }
        if (x.first.compare(0, keySize, var) != 0) {
            continue;
        }
        l->insert(l->begin(), new Variable(x.first, x.second));
    }
}


void InMemoryPerProcess::resolveRegularExpression(const std::string& var,
    std::vector<const Variable *> *l) {
    /* Not ready */
}


std::string* InMemoryPerProcess::resolveFirst(const std::string& var) {
    auto range = equal_range(var);

    for (auto it = range.first; it != range.second; ++it) {
        return &it->second;
    }

    return NULL;
}


void InMemoryPerProcess::store(std::string key, std::string compartment,
    std::string value) {
    std::string nkey = key + "::" + compartment;
    store(nkey, value);
}


bool InMemoryPerProcess::storeOrUpdateFirst(const std::string &key,
    std::string compartment, const std::string &value) {
    std::string nkey = key + "::" + compartment;
    return storeOrUpdateFirst(nkey, value);
}


bool InMemoryPerProcess::updateFirst(const std::string &key,
    std::string compartment, const std::string &value) {
    std::string nkey = key + "::" + compartment;
    return updateFirst(nkey, value);
}


void InMemoryPerProcess::del(const std::string& key,
    std::string compartment) {
    std::string nkey = key + "::" + compartment;
    del(nkey);
}


std::string* InMemoryPerProcess::resolveFirst(const std::string& var,
    std::string compartment) {
    std::string nkey = var + "::" + compartment;
    return resolveFirst(nkey);
}


void InMemoryPerProcess::resolveSingleMatch(const std::string& var,
    std::string compartment, std::vector<const Variable *> *l) {
    std::string nkey = var + "::" + compartment;
    resolveSingleMatch(nkey, l);
}


void InMemoryPerProcess::resolveMultiMatches(const std::string& var,
    std::string compartment, std::vector<const Variable *> *l) {
    std::string nkey = var + "::" + compartment;
    resolveMultiMatches(nkey, l);
}


void InMemoryPerProcess::resolveRegularExpression(const std::string& var,
    std::string compartment, std::vector<const Variable *> *l) {
    std::string nkey = var + "::" + compartment;
    resolveRegularExpression(nkey, l);
}


}  // namespace backend
}  // namespace collection
}  // namespace modsecurity