//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2018
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/telegram/UserId.h"

#include "td/utils/common.h"
#include "td/utils/Slice.h"
#include "td/utils/Status.h"
#include "td/utils/StringBuilder.h"
#include "td/utils/tl_helpers.h"

#include "td/telegram/secret_api.h"
#include "td/telegram/td_api.h"
#include "td/telegram/telegram_api.h"

#include <tuple>
#include <unordered_set>
#include <utility>

namespace td {

class MessageEntity {
  tl_object_ptr<td_api::TextEntityType> get_text_entity_type_object() const;

 public:
  enum class Type : int32 {
    Mention,
    Hashtag,
    BotCommand,
    Url,
    EmailAddress,
    Bold,
    Italic,
    Code,
    Pre,
    PreCode,
    TextUrl,
    MentionName
  };
  Type type;
  int32 offset;
  int32 length;
  string argument;
  UserId user_id;

  MessageEntity() = default;

  MessageEntity(Type type, int32 offset, int32 length, string argument = "")
      : type(type), offset(offset), length(length), argument(std::move(argument)), user_id() {
  }
  MessageEntity(int32 offset, int32 length, UserId user_id)
      : type(Type::MentionName), offset(offset), length(length), argument(), user_id(user_id) {
  }

  tl_object_ptr<td_api::textEntity> get_text_entity_object() const;

  bool operator==(const MessageEntity &other) const {
    return offset == other.offset && length == other.length && type == other.type && argument == other.argument &&
           user_id == other.user_id;
  }

  bool operator<(const MessageEntity &other) const {
    return std::tie(offset, length, type) < std::tie(other.offset, other.length, other.type);
  }

  bool operator!=(const MessageEntity &rhs) const {
    return !(*this == rhs);
  }

  // TODO move to hpp
  template <class StorerT>
  void store(StorerT &storer) const {
    using td::store;
    store(type, storer);
    store(offset, storer);
    store(length, storer);
    if (type == Type::PreCode || type == Type::TextUrl) {
      store(argument, storer);
    }
    if (type == Type::MentionName) {
      store(user_id, storer);
    }
  }

  template <class ParserT>
  void parse(ParserT &parser) {
    using td::parse;
    parse(type, parser);
    parse(offset, parser);
    parse(length, parser);
    if (type == Type::PreCode || type == Type::TextUrl) {
      parse(argument, parser);
    }
    if (type == Type::MentionName) {
      parse(user_id, parser);
    }
  }
};

StringBuilder &operator<<(StringBuilder &string_builder, const MessageEntity &message_entity);

const std::unordered_set<Slice, SliceHash> &get_valid_short_usernames();

vector<tl_object_ptr<td_api::textEntity>> get_text_entities_object(const vector<MessageEntity> &entities);

// sorts entities, removes intersecting and empty entities
void fix_entities(vector<MessageEntity> &entities);

vector<MessageEntity> find_entities(Slice text, bool skip_bot_commands, bool only_urls = false);

vector<MessageEntity> merge_entities(vector<MessageEntity> old_entities, vector<MessageEntity> new_entities);

vector<Slice> find_mentions(Slice str);
vector<Slice> find_bot_commands(Slice str);
vector<Slice> find_hashtags(Slice str);
bool is_email_address(Slice str);
vector<std::pair<Slice, bool>> find_urls(Slice str);  // slice + is_email_address

string get_first_url(Slice text, const vector<MessageEntity> &entities);

Result<vector<MessageEntity>> parse_markdown(string &text);

Result<vector<MessageEntity>> parse_html(string &text);

class ContactsManager;

vector<tl_object_ptr<telegram_api::MessageEntity>> get_input_message_entities(const ContactsManager *contacts_manager,
                                                                              const vector<MessageEntity> &entities);

vector<tl_object_ptr<secret_api::MessageEntity>> get_input_secret_message_entities(
    const vector<MessageEntity> &entities);

vector<MessageEntity> get_message_entities(vector<tl_object_ptr<telegram_api::MessageEntity>> &&server_entities);

vector<MessageEntity> get_message_entities(vector<tl_object_ptr<secret_api::MessageEntity>> &&secret_entities);

}  // namespace td
