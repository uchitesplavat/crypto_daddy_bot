//
// Created by up on 3/19/23.
//

#include <iostream>
#include <tgbot/tgbot.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iomanip>

std::string getTop10Crypto() {
    std::string api_url = "https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=10&page=1&sparkline=false";
    auto response = cpr::Get(cpr::Url{api_url});

    if (response.status_code == 200) {
        nlohmann::json json_data = nlohmann::json::parse(response.text);
        std::string formatted_data;

        for (const auto &coin : json_data) {
            std::string name = coin["name"].get<std::string>();
            std::string symbol = coin["symbol"].get<std::string>();
            std::string market_cap_rank = std::to_string(coin["market_cap_rank"].get<int>());
            double price = coin["current_price"].get<double>();

            std::stringstream price_stream;
            price_stream << std::fixed << std::setprecision(4) << price;

            formatted_data += "Rank " + market_cap_rank + ": " + name + " (" + symbol + ") - Price: $" + price_stream.str() + "\n";
        }

        return formatted_data;
    } else {
        return "Error fetching data from CoinGecko.";
    }
}

int main() {
    TgBot::Bot bot("6290187456:AAE5MO_JTBrQHPUfh_hwqW72B1NDC7BIp7g");

    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

        TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
        button1->text = "TRADING";
        button1->url = "https://telegra.ph/Trading-03-19";

        TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
        button2->text = "NFT";
        button2->url = "https://telegra.ph/Nft-03-19-3";

        TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
        button3->text = "TOP 10 CRYPTO";
        button3->callbackData = "top10crypto";

        std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
        row1.push_back(button1);
        row1.push_back(button2);

        std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
        row2.push_back(button3);

        keyboard->inlineKeyboard.push_back(row1);
        keyboard->inlineKeyboard.push_back(row2);

        bot.getApi().sendMessage(message->chat->id, "В чем бы вам хотелось разобраться?", false, 0, keyboard);
    });

    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "top10crypto") {
            std::string top10CryptoMsg = getTop10Crypto();
            bot.getApi().sendMessage(query->message->chat->id, top10CryptoMsg);
        }
    });

    try {
        std::cout << "Bot username: " << bot.getApi().getMe()->username << std::endl;
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            std::cout << "Long poll started" << std::endl;
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 0;
}

//    bot.getEvents().onCommand("crypto", [&bot](TgBot::Message::Ptr message) {
//        // Make an HTTP request to the CoinGecko API to retrieve the top 10 cryptocurrency prices
//        httplib::Client client("api.coingecko.com", 443);
//        auto res = client.Get("/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=10&page=1&sparkline=false");
//
//        // Parse the JSON response
//        std::vector<std::string> cryptocurrencies;
//        nlohmann::json_abi_v3_11_2::json response = nlohmann::json_abi_v3_11_2::json::parse(res->body);
//        for (const auto& item : response) {
//            std::string name = item["name"];
//            double price = item["current_price"];
//            std::string symbol = item["symbol"];
//            std::stringstream ss;
//            ss << name << " (" << symbol << "): $" << price;
//            cryptocurrencies.push_back(ss.str());
//        }
//
//        // Join the cryptocurrency strings together and send them as a message to the user
//        std::string message_text = "Top 10 cryptocurrencies by market cap:\n\n";
//        message_text += std::accumulate(cryptocurrencies.begin(), cryptocurrencies.end(), std::string(),
//                                        [](std::string& a, std::string& b) -> std::string {
//                                            return a + "\n" + b;
//                                        });
//        bot.getApi().sendMessage(message->chat->id, message_text);
//    });