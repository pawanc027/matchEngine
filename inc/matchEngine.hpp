#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace marketdata {

	const std::string BUY          = "BUY";
	const std::string SELL         = "SELL";

	struct order
	{
		uint64_t     timestamp;
		std::string  order_id;
		std::string  side;
		std::string  instrument;
		uint64_t     quantity;
		uint64_t     price;

		order()
		{
			timestamp = 0;
			quantity = 0;
			price = 0;
		}

		void clear()
		{ 
			timestamp = 0;
			order_id = "";
			side = "";
			quantity = 0;
			price = 0;
		}
	};

	using order_t        = order ;
	using bookvalue_t    = std::map< uint64_t, std::list< order_t > , std::greater < uint64_t> > ;
	using book_t         = std::unordered_map<std::string, bookvalue_t >;
	using simpleBook_t   = std::unordered_map < std::string, order_t  > ;

	class matchEngine { 
		public:
			matchEngine();
			matchEngine(const matchEngine& src) = delete;
			matchEngine& operator=(const matchEngine& rhs) = delete;

			virtual void pushOrder(const order_t& newOrder);
			virtual ~matchEngine();

		protected:
			void processOrder();
			void addOrder(const order_t& addOrder);
			void addBuyOrder(const order_t& addOrder);
			void addSellOrder(const order_t& addOrder);
			bool matchOrder(const order_t& newOrder);
			bool sellMatchOrder(const order_t& newOrder);
			bool buyMatchOrder(const order_t& newOrder);
			void reportOpenOrders() const;

		private:
			book_t askBook_;
			book_t bidBook_;
			simpleBook_t askSimpleBook_;
			simpleBook_t bidSimpleBook_;

			std::queue<order_t> orderQueue_;
			std::mutex mutex_;
			std::condition_variable condVar_;
			std::mutex coutMutex_;
			std::thread thread_;
			bool exit_ = false;
	};
}
