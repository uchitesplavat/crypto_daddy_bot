//
// Created by up on 3/19/23.
//

#include <iostream>
#include <tgbot/tgbot.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <sstream>
#include "pugixml.hpp"

std::string escapeMarkdownV2(const std::string& input) {
    std::string result = input;
    std::string charsToEscape = "_*[]()~`>#+-=|{}.!";
    for (char c : charsToEscape) {
        std::string charStr(1, c);
        size_t pos = result.find(charStr);
        while (pos != std::string::npos) {
            result.replace(pos, 1, "\\" + charStr);
            pos = result.find(charStr, pos + 2);
        }
    }
    return result;
}

//std::string getNews() {
//    std::string api_url = "https://www.coindesk.com/feed";
//    auto response = cpr::Get(cpr::Url{api_url});
//
//    if (response.status_code == 200) {
//        pugi::xml_document doc;
//        pugi::xml_parse_result result = doc.load_string(response.text.c_str());
//
//        if (!result) {
//            return "Error parsing XML data.";
//        }
//
//        std::string formatted_data;
//        int news_counter = 1;
//
//        for (const auto &item : doc.child("rss").child("channel").children("item")) {
//            std::string title = item.child_value("title");
//            std::string url = item.child_value("link");
//
//            title = escapeMarkdownV2(title);
//
//            formatted_data += std::to_string(news_counter) + ". [" + title + "](" + url + ")\n";
//            news_counter++;
//
//            if (news_counter > 5) {
//                break;
//            }
//        }
//
//        return formatted_data;
//    } else {
//        return "Error fetching data from CoinDesk.";
//    }
//}

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

            std::string price_string = price_stream.str();
            // Escape special characters for MarkdownV2
            price_string = escapeMarkdownV2(price_string);

            // Use MarkdownV2 formatting and escape parentheses, hyphen, and dot in the price
            // Add double backslashes for newlines
            formatted_data += "Rank " + market_cap_rank + ": *" + name + "* \\(" + symbol + "\\) \\- Price: $" + price_string + "\n";
        }

        return formatted_data;
    } else {
        return "Error fetching data from CoinGecko.";
    }
}



bool contains_prohibited_words(const std::string& message, const std::vector<std::string>& prohibited_words) {
    for (const auto& word : prohibited_words) {
        if (message.find(word) != std::string::npos) {
            return true;
        }
    }
    return false;
}

int main() {
    TgBot::Bot bot("api_key");

    std::vector<std::string> prohibited_words = {prohibited_words};

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

        TgBot::InlineKeyboardButton::Ptr button4(new TgBot::InlineKeyboardButton);
        button4->text = "DONATE";
        button4->url = "https://telegra.ph/DONATE-03-21-2";

//        TgBot::InlineKeyboardButton::Ptr button5(new TgBot::InlineKeyboardButton);
//        button5->text = "NEWS";
//        button5->callbackData = "news";


        std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
        row1.push_back(button1);
        row1.push_back(button2);

        std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
        row2.push_back(button3);

        std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
        row3.push_back(button4);

//        std::vector<TgBot::InlineKeyboardButton::Ptr> row4;
//        row4.push_back(button5);


        keyboard->inlineKeyboard.push_back(row1);
        keyboard->inlineKeyboard.push_back(row2);
        keyboard->inlineKeyboard.push_back(row3);
//        keyboard->inlineKeyboard.push_back(row4);

        bot.getApi().sendMessage(message->chat->id, "В чем бы вам хотелось разобраться?", false, 0, keyboard);
    });

    bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
        if (query->data == "top10crypto") {
            std::string top10CryptoMsg = getTop10Crypto();
            bot.getApi().sendMessage(query->message->chat->id, top10CryptoMsg, false, 0, nullptr, "MarkdownV2");
        }
//        if (query->data == "news") {
//            std::string newsMsg = getNews();
//            bot.getApi().sendMessage(query->message->chat->id, newsMsg, false, 0, nullptr, "MarkdownV2");
//        }
    });
    bot.getEvents().onAnyMessage([&bot, &prohibited_words](TgBot::Message::Ptr message) {
        if (contains_prohibited_words(message->text, prohibited_words)) {
            try {
                bot.getApi().deleteMessage(message->chat->id, message->messageId);
            } catch (TgBot::TgException& e) {
                std::cerr << "Error deleting message: " << e.what() << std::endl;
            }
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
