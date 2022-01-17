#pragma once
#include "NetCommon.h"
#include "NetQueue.h"
#include "NetPacket.h"
#include "NetConnection.h"


namespace NetLib {

	template <typename T>
	class ClientApp {
	public:
		ClientApp() {

		}
		virtual ~ClientApp() {
			Disconnect();
		}

		bool Connect(const std::string& hostname, const uint16_t port) {
			try
			{
				asio::ip::tcp::resolver resolver(m_ASIOContext);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(hostname, std::to_string(port));
				
				m_Connection = std::make_unique<Connection<T>>(Connection<T>::ConnectType::Client, m_ASIOContext, asio::ip::tcp::socket(m_ASIOContext), m_PktIn);
				m_Connection->ConnectToServer(endpoints);
				m_threadContext = std::thread([this]() { m_ASIOContext.run(); });
				Sleep(2000);
				
			}
			catch (std::exception& e)
			{
				std::cerr << "Client Exception: " << e.what() << "\n";
				return false;
			}
			return true;
		}
		void Disconnect() {
			if (IsConnected())
			{

				m_Connection->Disconnect();
			}
			m_ASIOContext.stop();
			if (m_threadContext.joinable())
				m_threadContext.join();
			m_Connection.release();
		}
		bool IsConnected() {
			return m_Connection ? m_Connection->IsConnected() : false;
		}
		void SendToServer(const Packet<T>& pkt) {
			if (IsConnected())
				m_Connection->SendPacket(pkt);
		}

		NetQueue<OwnedPacket<T>>& GetIncomingPktQue() { return m_PktIn; }
	protected:
		asio::io_context				m_ASIOContext;
		std::thread						m_threadContext;
		std::unique_ptr<Connection<T>>	m_Connection;
	private:
		NetQueue<OwnedPacket<T>>		m_PktIn;
	};
}