/**************************************************************************
 *                                                                        *
 *  Description: MinimalAgent multi-agent framework                       *
 *  Website:     https://github.com/jferdelyi/MinimalAgent                *
 *  Copyright:   (c) 2023-Today, Jean-François Erdelyi                    *
 *                                                                        *
 *  CPP version of ActressMAS by Florin Leon                              *
 *  https://github.com/florinleon/ActressMas                              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or modify  *
 *  it under the terms of the GNU General License as published by         *
 *  the Free Software Foundation. This program is distributed in the      *
 *  hope that it will be useful, but WITHOUT ANY WARRANTY; without even   *
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR   *
 *  PURPOSE. See the GNU General License for more details.                *
 *                                                                        *
 **************************************************************************/

#pragma once

#include <godot_cpp/core/class_db.hpp>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace godot {

/**
 * Message binary format
 */
enum class MessageBinaryFormat {
    RAW, BJData, BSON, CBOR, MessagePack, UBJSON
};

/**
 * A message that the agents use to communicate. In an agent-based system, the
 * communication between the agents is exclusively performed by exchanging
 * messages.
 **/
class Message final {

protected:
    /**
     * Sender.
     **/
    String m_sender;

    /**
     * Receiver.
     **/
    String m_receiver;

    /**
     * Binary format.
     **/
    MessageBinaryFormat m_binary_format;

    /**
     * Raw message.
     **/
    std::vector<std::uint8_t> m_binary_message;
    String m_message;

public:
    /**
     * Message.
     * @param p_sender Sender.
     * @param p_receiver Receiver.
     * @param p_message Message.
     * @param p_binary_format Binary format used.
     **/
    Message(String p_sender, String p_receiver, const String& p_message, const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::RAW);
    Message();

    /**
     * Nothing to delete.
     **/
    /*virtual*/ ~Message() = default;

    /**
     * Get sender.
     * @return Sender.
     **/
    [[nodiscard]] String get_sender() const { return m_sender; }

    /**
     * Get receiver.
     * @return Receiver.
     **/
    [[nodiscard]] String get_receiver() const { return m_receiver; }

    /**
     * Get binary message.
     * @return binary message JSON
     **/
    [[nodiscard]] const std::vector<std::uint8_t>& get_binary_message() const { return m_binary_message; }

    /**
     * Get binary message.
     * @return binary message JSON
     **/
    [[nodiscard]] const MessageBinaryFormat& get_binary_format() const { return m_binary_format; }

    /**
     * Get message.
     * @return message JSON
     **/
    [[nodiscard]] json content() const;

    /**
     * Format message to string.
     * @return string message from JSON
     **/
    [[nodiscard]] String to_string() const;

    /**
     * Format message.
     * @return string formated message
     **/
    [[nodiscard]] String format() const;

    /**
     * From/to json/binary.
     * @param p_message json/binary message
     * @param p_binary_format binary format
     * @return json/binary
     **/
    static std::vector<std::uint8_t> to_binary(const json& p_message, const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::MessagePack);
    static json to_json(const std::vector<std::uint8_t>& p_message, const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::MessagePack);

    // Delete copy constructor
    Message(const Message&) = delete;

    Message& operator=(Message&) = delete;
};

// Message pointer
using MessagePointer = std::shared_ptr<const Message>;
}
