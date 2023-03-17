#include "eoscommerce3.hpp"

const std::string_view waxString{"WAX"};
const uint8_t waxdecimals = 8;
const symbol waxsymbol(waxString, waxdecimals);

const std::string_view ecom_string{"ECOM"};
const uint8_t ecom_decimals = 4;
const symbol ecom_symbol(ecom_string, ecom_decimals);


[[eosio::action]]
void eoscommerce3::erase() {
  require_auth(_self); // Require authorization of the contract owner

  account_balance_table balances(_self, _self.value);

  // Iterate through the table and erase each entry
  auto itr = balances.begin();
  while (itr != balances.end()) {
    itr = balances.erase(itr);
  }
}


[[eosio::on_notify("eoscommtoken::transfer")]]
void eoscommerce3::on_transfer(name from, name to, eosio::asset quantity, std::string memo) {
		const eosio::name REAL_CONTRACT = "eoscommtoken"_n;
		const eosio::name ORIGINAL_CONTRACT = get_first_receiver();	

		

		check( quantity.amount >= 10000, "Quanity must be greater than 1 ECOM" );
		check( REAL_CONTRACT == ORIGINAL_CONTRACT, "You tryna get over on us, bro?" );

		if( from == get_self() || to != get_self() ){ return; }

		check( quantity.symbol == ecom_symbol, "Symbol is not what we were expecting" );

		//emplace info into table
		account_balance_table balance( get_self(), get_self().value );
		auto itr = balance.find( from.value );

		if( itr != balance.end() ){
			//modify this user's balance
			balance.modify( itr, same_payer, [&](auto &row) {
				row.ecom_balance += quantity;
			});
		} else {
			balance.emplace( get_self(), [&](auto &row) {

				row.user = from.value;
        		row.username = from;
				row.ecom_balance = quantity;
				row.wax_balance = asset(0, waxsymbol);

			});

		}

		// action(permission_level{to, "active"_n}, 
		// "eoscommtoken"_n, "transfer"_n, 
		// std::make_tuple(eosio::name("eoscommerce3"), from, asset(100000, waxsymbol), std::string("We just reacted son"))).send();

        
}
