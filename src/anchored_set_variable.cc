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

#include "modsecurity/anchored_set_variable.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "modsecurity/modsecurity.h"
#include "modsecurity/transaction.h"
#include "src/utils/regex.h"

namespace modsecurity {


AnchoredSetVariable::AnchoredSetVariable(Transaction *t, std::string name)
    : m_transaction(t),
    m_name(name) {
        reserve(10);
    }


AnchoredSetVariable::~AnchoredSetVariable() {
    unset();
}


void AnchoredSetVariable::unset() {
    for (const auto& x : *this) {
        collection::Variable *var = x.second;
        delete var->m_key;
        var->m_key = NULL;
        delete var;
    }
    clear();
}


void AnchoredSetVariable::set(const std::string &key,
    const std::string &value, size_t offset, size_t len) {
    std::unique_ptr<VariableOrigin> origin(new VariableOrigin());
    std::string *v = new std::string(value);
    std::string *k = new std::string(m_name + ":" + key);
    collection::Variable *var = new collection::Variable(k, v);

    origin->m_offset = offset;
    origin->m_length = len;

    var->m_dynamic_value = true;
    var->m_dynamic = false;
    var->m_orign.push_back(std::move(origin));
    emplace(key, var);
}


void AnchoredSetVariable::set(const std::string &key,
    const std::string &value, size_t offset) {
    std::unique_ptr<VariableOrigin> origin(new VariableOrigin());
    std::string *v = new std::string(value);
    std::string *k = new std::string(m_name + ":" + key);
    collection::Variable *var = new collection::Variable(k, v);

    origin->m_offset = offset;
    origin->m_length = value.size();

    var->m_dynamic_value = true;
    var->m_dynamic = false;
    var->m_orign.push_back(std::move(origin));
    emplace(key, var);
}


void AnchoredSetVariable::resolve(
    std::vector<const collection::Variable *> *l) {
    for (const auto& x : *this) {
        l->insert(l->begin(), x.second);
    }
}


void AnchoredSetVariable::resolve(const std::string &key,
    std::vector<const collection::Variable *> *l) {
    auto range = this->equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        l->push_back(it->second);
    }
}


std::unique_ptr<std::string> AnchoredSetVariable::resolveFirst(
    const std::string &key) {
    auto range = equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        std::unique_ptr<std::string> b(new std::string());
        b->assign(*it->second->m_value);
        return b;
    }
    return nullptr;
}


void AnchoredSetVariable::resolveRegularExpression(Utils::Regex *r,
    std::vector<const collection::Variable *> *l) {
    for (const auto& x : *this) {
        int ret = Utils::regex_search(x.first, *r);
        if (ret <= 0) {
            continue;
        }
        l->insert(l->begin(), x.second);
    }
}


}  // namespace modsecurity