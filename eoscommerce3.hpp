#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/symbol.hpp>
#include <string>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>


using namespace eosio;

class [[eosio::contract("eoscommerce3")]] eoscommerce3 : public eosio::contract {
  public:
    using contract::contract;
	  eoscommerce3(name receiver, name code, datastream<const char *> ds):contract(receiver, code, ds){}
    // Define the table structure to store account balances
    struct [[eosio::table]] balances 
    {
		uint64_t            user;
    name                username;
		eosio::asset        ecom_balance;
    eosio::asset        wax_balance;
		uint64_t primary_key() const { return user; }
	  };

     // Create a type alias for the account_balance table
    using account_balance_table = eosio::multi_index<"balances"_n, balances>;


    struct [[eosio::table]] rewards
    {
      uint64_t            user;          // Primary key: user account name
      eosio::asset        wax_balance;   // WAX balance

      // Getter for the primary key
      uint64_t primary_key() const { return user; }
	  };
    using rewards_balance_table = eosio::multi_index<"rewards"_n, rewards>;
   
    [[eosio::on_notify("eoscommtoken::transfer")]]
    void on_transfer(name from, name to, asset quantity, std::string memo);

    [[eosio::action]]
    void addrewards(asset quantity, name nft_owner);

    [[eosio::action]]
    void giverewards();

    [[eosio::action]]
    void erase();

    asset total_ecom();

    float ecomwax_ratio(name _self);

};
