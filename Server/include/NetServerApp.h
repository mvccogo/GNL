#pragma once
#include "NetCommon.h"
#include "NetQueue.h"
#include "NetPacket.h"
#include "NetTCPConnection.h"


namespace NetLib {
	template <typename T>
	class ServerApp {
	public:
		ServerApp(uint16_t port)
			: m_ASIOAcceptor(m_ASIOContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port), true)
		{
	


		}
			
		~ServerApp()
		{
			Stop();
		}

		bool Start() {
			try
			{
				AwaitTCPConnection();
				m_threadContext = std::thread([this]() { m_ASIOContext.run(); });
			}
			catch (std::exception& e)
			{
				std::cerr << "[SERVER] Exception: " << e.what() << "\n";
				return false;
			}

			std::cout << "[SERVER] Started!\n";
			return true;
		}
		void Stop() {
			m_ASIOContext.stop();

			if (m_threadContext.joinable()) m_threadContext.join();
			std::cout << "[SERVER] Stopped!\n";
		}
		void AwaitTCPConnection() {
			m_ASIOAcceptor.async_accept(
				[this](std::error_code ec, asio::ip::tcp::socket socket)
				{
					if (!ec)
					{
						std::cout << "[SERVER] New TCPConnection: " << socket.remote_endpoint() << "\n";

						std::shared_ptr<TCPConnection<T>> newconn =
							std::make_shared<TCPConnection<T>>(TCPConnection<T>::ConnectType::Server,
								m_ASIOContext, std::move(socket), m_PktIn);

						if (OnClientConnect(newconn))
						{
							m_TCPConnections.push_back(move(newconn));
							m_TCPConnections.back()->ConnectToClient(m_IDCnt++);
							
							std::cout << "[" << m_TCPConnections.back()->GetID() << "] TCPConnection Approved\n";
						}
						else
						{
							std::cout << "[-----] TCPConnection Denied\n";
						}
					}
					else
					{
						std::cout << "[SERVER] New TCPConnection Error: " << ec.message() << "\n";
					}
					AwaitTCPConnection();
				});
		}
		void SendToClient(std::shared_ptr<TCPConnection<T>>& client, const Packet<T>& pkt) {
			if (client && client->IsConnected())
			{
				client->SendPacket(pkt);
			}
			else
			{
				OnClientDisconnect(client);

				m_TCPConnections.erase(std::remove(m_TCPConnections.begin(), m_TCPConnections.end(), client), m_TCPConnections.end());
				client = nullptr;
				std::cout << "TCPConnection deque after removal: " << std::endl;
				for (int i = 0; i < m_TCPConnections.size(); i++) {
					std::cout << "[" << m_TCPConnections.at(i)->GetID() << "]\n";

				}
			}
		}
		void SendToAll(const Packet<T>& pkt, std::shared_ptr<TCPConnection<T>>& ignore_client = nullptr) {
			bool bInvalidClientExists = false;

			for (auto& client : m_TCPConnections)
			{
				if (client && client->IsConnected())
				{
					if (client != ignore_client)
						client->SendPacket(pkt);
				}
				else
				{
					OnClientDisconnect(client);
					client = nullptr;
					bInvalidClientExists = true;
				}
			}

			if (bInvalidClientExists) {
				m_TCPConnections.erase(
					std::remove(m_TCPConnections.begin(), m_TCPConnections.end(), nullptr), m_TCPConnections.end());
				
			}
		}
		void Process(size_t max_pkt = -1, bool wait = false) {
			if (wait) m_PktIn.wait();

			size_t nMessageCount = 0;
			while (nMessageCount < max_pkt && !m_PktIn.is_empty())
			{
				auto pkt = m_PktIn.pop_front();
				OnPacket(pkt.connectionPtr, pkt.pkt);

				nMessageCount++;
			}
		}
	protected:
		virtual bool OnClientConnect(std::shared_ptr<TCPConnection<T>>& client) { return false; }
		virtual void OnClientDisconnect(std::shared_ptr<TCPConnection<T>>& client){ };
		virtual void OnPacket(std::shared_ptr<TCPConnection<T>>& client, Packet<T>& pkt){};


		NetQueue<OwnedPacket<T>>						m_PktIn;
		std::deque<std::shared_ptr<TCPConnection<T>>>		m_TCPConnections;
		asio::io_context								m_ASIOContext;
		asio::ip::tcp::acceptor							m_ASIOAcceptor;
		uint32_t										m_IDCnt;
		std::thread										m_threadContext;



	};

}
