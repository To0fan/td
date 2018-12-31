//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/mtproto/AuthData.h"

#include "td/telegram/net/DcId.h"
#include "td/telegram/net/PublicRsaKeyShared.h"

#include "td/utils/common.h"
#include "td/utils/StringBuilder.h"

#include <memory>
#include <utility>

namespace td {
enum class AuthState : int32 { Empty, KeyNoAuth, OK };

inline StringBuilder &operator<<(StringBuilder &sb, AuthState state) {
  switch (state) {
    case AuthState::Empty:
      return sb << "Empty";
    case AuthState::KeyNoAuth:
      return sb << "KeyNoAuth";
    case AuthState::OK:
      return sb << "OK";
    default:
      return sb << "Unknown AuthState";
  }
}

class AuthDataShared {
 public:
  virtual ~AuthDataShared() = default;
  class Listener {
   public:
    Listener() = default;
    Listener(const Listener &) = delete;
    Listener &operator=(const Listener &) = delete;
    virtual ~Listener() = default;
    virtual bool notify() = 0;
  };

  virtual DcId dc_id() const = 0;
  virtual const std::shared_ptr<PublicRsaKeyShared> &public_rsa_key() = 0;
  virtual mtproto::AuthKey get_auth_key() = 0;
  virtual std::pair<AuthState, bool> get_auth_state() = 0;
  virtual void set_auth_key(const mtproto::AuthKey &auth_key) = 0;
  virtual void update_server_time_difference(double diff) = 0;
  virtual double get_server_time_difference() = 0;
  virtual void add_auth_key_listener(unique_ptr<Listener> listener) = 0;

  virtual void set_future_salts(const std::vector<mtproto::ServerSalt> &future_salts) = 0;
  virtual std::vector<mtproto::ServerSalt> get_future_salts() = 0;

  static AuthState get_auth_state(const mtproto::AuthKey &auth_key) {
    AuthState state;
    if (auth_key.empty()) {
      state = AuthState::Empty;
    } else if (auth_key.auth_flag()) {
      state = AuthState::OK;
    } else {
      state = AuthState::KeyNoAuth;
    }
    return state;
  }

  static std::shared_ptr<AuthDataShared> create(DcId dc_id, std::shared_ptr<PublicRsaKeyShared> public_rsa_key);
};

};  // namespace td