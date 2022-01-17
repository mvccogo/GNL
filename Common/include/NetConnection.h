#pragma once
#include "NetCommon.h"
#include "NetPacket.h"
#include "NetQueue.h"

namespace NetLib 
{
	template <typename T>
	class Connection : public std::enable_shared_from_this<Connection<T>>
	{

		friend class NetQueue<T>;
	public:
		enum class ConnectType : char {
			Server,
			Client
		};
		Connection(ConnectType type, asio::io_context& asioContext, asio::ip::tcp::socket socket, NetQueue<OwnedPacket<T>>& Receiver) 
			: m_ASIOContext(asioContext), m_Socket(std::move(socket)), m_PktIn(Receiver)
		{
			m_Type = type;
		}

		~Connection() {};

		uint32_t GetID() const { return m_sID; }

		void ConnectToClient(uint32_t clientID = 0) {
			if (m_Type == ConnectType::Server) {
				if (m_Socket.is_open()) {
					m_sID = clientID;
					ReadHeader();

				}

			}
		}

		void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
			if (m_Type == ConnectType::Client)
			{
				asio::async_connect(m_Socket, endpoints,
					[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
					{
						if (!ec)
						{
							ReadHeader();
						}
					});
			}
		}


		void Disconnect() {
			if (IsConnected())
				asio::post(m_ASIOContext,
					[this]() {
						m_Socket.close();

					});
		}
		bool IsConnected() const { return m_Socket.is_open(); }
		void StartListening() {};


		void SendPacket(const Packet<T>& pkt) {
			//std::unique_lock<std::mutex> ul(m_PktOut.GetQMutex());
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
			asio::async_write(m_Socket, asio::buffer(&m_PktOut.get_front().header, sizeof(PacketHeader<T>)),
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
			asio::async_write(m_Socket, asio::buffer(m_PktOut.get_front().body.data(), m_PktOut.get_front().body.size()),
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
			asio::async_read(m_Socket, asio::buffer(&m_tmpPktIn.header, sizeof(PacketHeader<T>)),
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
			asio::async_read(m_Socket, asio::buffer(m_tmpPktIn.body.data(), m_tmpPktIn.body.size()),
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
				m_PktIn.push_back({ this->shared_from_this(), m_tmpPktIn });
			else
				m_PktIn.push_back({ nullptr, m_tmpPktIn });

			ReadHeader();
		}





		
		
	private:
		
	protected:
		uint32_t					m_sID;
		asio::ip::tcp::socket		m_Socket;
		asio::io_context&			m_ASIOContext;
		ConnectType					m_Type;
		Packet<T>					m_tmpPktIn;
		NetQueue<Packet<T>>			m_PktOut;
		
		NetQueue<OwnedPacket<T>>&	m_PktIn;


	};
}