#pragma once
#include <eosiolib/contracts/eosio/action.hpp>
#include <eosiolib/contracts/eosio/eosio.hpp>
#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/symbol.hpp>
#include <string>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <string_view>

using namespace eosio;

class [[eosio::contract]] eoscommerce1 : public contract {
  public:
    using contract::contract;

    // Define the table structure to store account balances
    struct [[eosio::table]] account_balances {
      name account;
      asset ecom_balance;
      asset wax_balance;
      uint64_t primary_key() const { return account.value; }
    };

    // Create a type alias for the account_balance table
    using account_balance_table = eosio::multi_index<"balances"_n, account_balances>;

    [[eosio::action]]
    void addbalance(name account, asset quantity);

    [[eosio::on_notify("eoscommerce1::transfer")]]
    void on_transfer(name from, name to, asset quantity, std::string memo);

  private:
    void check_transfer(name contract, asset quantity);
};
