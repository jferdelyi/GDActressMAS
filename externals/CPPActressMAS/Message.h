/**************************************************************************
 *                                                                        *
 *  Description: CPPActressMas multi-agent framework                      *
 *  Website:     https://github.com/jferdelyi/CPPActressMAS               *
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

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include <cereal/types/vector.hpp> // Serialize vector
#include <cereal/types/string.hpp> // Serialize string
#include <cereal/types/memory.hpp> // Serialize smart pointers

/**
 * CPPActressMAS
 */
namespace cam {

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
		std::string m_sender;

		/**
		 * Receiver.
		 **/
		std::string m_receiver;

		/**
		 * Binary format.
		 **/
		MessageBinaryFormat m_binary_format;

		/**
		 * Raw message.
		 **/
		std::vector<std::uint8_t> m_binary_message;

	public:
		/**
		 * Message.
		 * @param p_sender Sender.
		 * @param p_receiver Receiver.
		 * @param p_message Message.
		 * @param p_binary_format Binary format used.
		 **/
		Message(std::string p_sender, std::string p_receiver, const json& p_message,
				const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::MessagePack);

		Message(std::string p_sender, std::string p_receiver, const uint8_t* p_message, const size_t& p_length,
				const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::RAW);

		Message();

		/**
		 * Nothing to delete.
		 **/
		/*virtual*/ ~Message() = default;

		/**
		 * Get sender.
		 * @return Sender.
		 **/
		[[nodiscard]] const std::string& get_sender() const { return m_sender; }

		/**
		 * Get receiver.
		 * @return Receiver.
		 **/
		[[nodiscard]] const std::string& get_receiver() const { return m_receiver; }

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
		[[nodiscard]] std::string to_string() const;

		/**
		 * Format message.
		 * @return string formated message
		 **/
		[[nodiscard]] /*virtual*/ std::string format() const;

		template<class Archive>
		void serialize(Archive& p_archive) {
			p_archive(m_sender, m_receiver, m_binary_message, m_binary_format);
		}

		/**
		 * From/to json/binary.
		 * @param p_message json/binary message
		 * @param p_binary_format binary format
		 * @return json/binary
		 **/
		static std::vector<std::uint8_t>
		to_binary(const json& p_message, const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::MessagePack);

		static json to_json(const std::vector<std::uint8_t>& p_message,
							const MessageBinaryFormat& p_binary_format = MessageBinaryFormat::MessagePack);

		// Delete copy constructor
		Message(const Message&) = delete;

		Message& operator=(Message&) = delete;
	};

	// Message pointer
	using MessagePointer = std::shared_ptr<const Message>;
} // namespace cam
