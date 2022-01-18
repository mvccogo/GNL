#pragma once
#include "NetCommon.h"
#include "NetPacket.h"
#include "NetQueue.h"

namespace NetLib {
	template <typename T>
	class UDP : public std::enable_shared_from_this<UDP<T>>
	{
		friend class NetQueue<T>;

	public:
		enum class ConnectType : char {
			Server,
			Client
		};
		UDP(ConnectType type, asio::io_context& asioContext, uint16_t port, asio::ip::udp::socket socket, NetQueue<Packet<T>>& Receiver) : m_PktIn(Receiver), m_Socket(std::move(socket)), m_ASIOContext(asioContext)
		{
			m_Type = type;
		}

		~UDP() {};

		void ConnectToClient(const asio::ip::udp::resolver::results_type& endpoints, uint32_t clientID = 0) {
			if (m_Type == ConnectType::Server) {
				if (m_Socket.is_open()) {
					m_sID = clientID;
					ReadHeader();

				}
				else {
					m_Socket.async_connect(endpoints,
						[this](std::error_code ec, asio::ip::udp::endpoint endpoint)
						{
							if (!ec)
							{
								ReadHeader();
							}
						});
				}

			}
		}

		void ConnectToServer(const asio::ip::udp::resolver::results_type& endpoints) {
			if (m_Type == ConnectType::Client)
			{
				m_Socket.async_connect(endpoints,
					[this](std::error_code ec, asio::ip::udp::endpoint endpoint)
					{
						if (!ec)
						{
							ReadHeader();
						}
					});
			}
		}


		void SendPacket(const Packet<T>& pkt) {
			std::unique_lock<std::mutex> ul(m_PktOut.GetQMutex());
			asio::post(m_ASIOContext,
				[this, pkt]()
				{
					bool bWritingMessage = !m_PktOut.is_empty();
					m_PktOut.push_back(pkt);
					if (!bWritingMessage)
					{
						WriteHeader();
					}

				});

		}
	private:

		void WriteHeader() {
			m_Socket.async_send(asio::buffer(&m_PktOut.get_front().header, sizeof(PacketHeader<T>)),
				[this](std::error_code ec, size_t length)
				{
					if (!ec)
					{

						if (m_PktOut.get_front().body.size() > 0)
						{
							WriteData();
						}
						else
						{
							m_PktOut.pop_front();
							if (!m_PktOut.is_empty())
							{

								WriteHeader();
							}
						}
					}
					else
					{

						std::cout << "[" << m_sID << "] Write Header Fail, error code: " << ec << "\n";
						m_Socket.close();
					}
				});
		}
		void WriteData() {
			m_Socket.async_send(asio::buffer(m_PktOut.get_front().body.data(), m_PktOut.get_front().body.size()),
				[this](std::error_code ec, size_t length)
				{
					if (!ec)
					{
						m_PktOut.pop_front();
						if (!m_PktOut.is_empty())
						{
							WriteHeader();
						}
					}
					else
					{
						std::cout << "[" << m_sID << "] Write Body Fail.\n";
						m_Socket.close();
					}
				});
		}
		void ReadHeader() {
			m_Socket.async_receive(asio::buffer(&m_tmpPktIn.header, sizeof(PacketHeader<T>)),
				[this](std::error_code ec, size_t length)
				{
					if (!ec)
					{
						if (m_tmpPktIn.header.size > 0)
						{
							m_tmpPktIn.body.resize(m_tmpPktIn.header.size);
							ReadData();
						}
						else
						{
							PushToIncomingQueue();
						}
					}
					else if (ec.value() == WSAECONNRESET) {
						// An existing connection was forcibly closed by the remote host.
						// No need to log this.
						m_Socket.close();
					}
					else
					{
						std::cout << "[" << m_sID << "] Read Header Fail, error code: " << ec.value() << ", message:" << ec.message() << "\n";
						m_Socket.close();
					}
				});
		}
		void ReadData() {
			m_Socket.async_receive(asio::buffer(m_tmpPktIn.body.data(), m_tmpPktIn.body.size()),
				[this](std::error_code ec, std::size_t length)
				{
					if (!ec)
					{
						PushToIncomingQueue();
					}
					else
					{

						std::cout << "[" << m_sID << "] Read Body Fail.\n";
						m_Socket.close();
					}
				});
		}
		void PushToIncomingQueue() {
			if (m_Type == ConnectType::Server)
				m_PktIn.push_back({ m_tmpPktIn });
			else
				m_PktIn.push_back({ m_tmpPktIn });

			ReadHeader();
		}


	protected:

		uint32_t					m_sID;
		asio::ip::udp::socket		m_Socket;
		asio::io_context&		 m_ASIOContext;
		asio::ip::udp::endpoint		m_Endpoint;
		Packet<T>					m_tmpPktIn;
		NetQueue<Packet<T>>			m_PktOut;
		NetQueue<Packet<T>>&		m_PktIn;
		ConnectType					m_Type;
	};

}