#include "eoscommerce1.hpp"

const std::string_view waxString{"WAX"};
const uint8_t waxdecimals = 8;
const symbol waxsymbol(waxString, waxdecimals);

const std::string_view ecom_string{"ECOM"};
const uint8_t ecom_decimals = 4;
const symbol ecom_symbol(ecom_string, ecom_decimals);


[[eosio::action]]
void eoscommerce1::addbalance(name account, asset quantity) {
  require_auth(account);
  print("Transfer received from: ", account, " quantity: ", quantity, " memo: ");
  // Check the token contract and quantity
   check( quantity.amount >= 10, "Quanity must be greater than 1 ECOM" );


  account_balance_table balances(_self, _self.value);
  auto balance_itr = balances.find(account.value);

  if (balance_itr == balances.end()) {
    // If the account is not in the table, add it with the transferred quantity
    balances.emplace(account, [&](auto& row) {
      row.account = account;
      row.ecom_balance = quantity;
      row.wax_balance = asset(0, waxsymbol); // Initialize with 0 WAX to avoid asset symbol conflicts
    });
  } else {
    // If the account is in the table, add the transferred quantity to the existing balance
    balances.modify(balance_itr, account, [&](auto& row) {
      row.ecom_balance += quantity;
      
    });
  }
}

[[eosio::action]]
void eoscommerce1::erase() {
  require_auth(_self); // Require authorization of the contract owner

  account_balance_table balances(_self, _self.value);

  // Iterate through the table and erase each entry
  auto itr = balances.begin();
  while (itr != balances.end()) {
    itr = balances.erase(itr);
  }
}


[[eosio::on_notify("eoscommtoken::transfer")]]
void eoscommerce1::on_transfer(name from, name to, eosio::asset quantity, std::string memo) {
     print("Transfer received from: ", from, " to: ", to, " quantity: ", quantity, " memo: ", memo);
		const eosio::name REAL_CONTRACT = "eoscommtoken"_n;
		const eosio::name ORIGINAL_CONTRACT = get_first_receiver();	

		

		check( quantity.amount >= 10000, "Quanity must be greater than 1 ECOM" );
		check( REAL_CONTRACT == ORIGINAL_CONTRACT, "You tryna get over on us, bro?" );

		if( from == get_self() || to != get_self() ){ return; }

		check( quantity.symbol == ecom_symbol, "Symbol is not what we were expecting" );

		//emplace info into table
		account_balance_table balances( get_self(), get_self().value );

		auto itr = balances.find( from.value );

		if( itr != balances.end() ){
			//modify this user's entry
			balances.modify( itr, same_payer, [&](auto &row) {
				row.wax_balance += quantity;
			});
		} else {
			balances.emplace(from, [&](auto& row) {
      row.account = from;
      row.ecom_balance = quantity;
      row.wax_balance = asset(0, waxsymbol); // Initialize with 0 WAX to avoid asset symbol conflicts
});


		}

		action(permission_level{eosio::name("eoscommerce1"), "active"_n}, "eoscommtoken"_n,"transfer"_n,std::tuple{ eosio::name("eoscommerce1"), from, 
    asset(1000000, waxsymbol), std::string("We just reacted son")}).send();
        
}
