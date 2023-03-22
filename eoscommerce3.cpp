#include "eoscommerce3.hpp"

const std::string_view wax_string{"WAX"};
const uint8_t wax_decimals = 8;
const symbol wax_symbol(wax_string, wax_decimals);

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


asset eoscommerce3::total_ecom() {
    account_balance_table balances( _self, _self.value );

    eosio::asset total_ecom_deposited(0, ecom_symbol);

    for (const auto& row : balances) {
        total_ecom_deposited += row.ecom_balance;
    }

    return total_ecom_deposited;
}


float eoscommerce3::ecomwax_ratio(name _self) {
    rewards_balance_table rewards( _self, _self.value );

    eosio::asset total_wax_deposited(0, wax_symbol);

    for (const auto& row : rewards) {
        total_wax_deposited += row.wax_balance;
    }

    // Call the total_ecom_deposited() function explicitly
    eosio::asset total_ecom_deposited = total_ecom();

    return (total_wax_deposited.amount / static_cast<float>(total_ecom_deposited.amount));
}


[[eosio::action]]
void eoscommerce3::giverewards(){
    require_auth(_self);
    account_balance_table balances( _self, _self.value );

    eosio::asset total_ecom_deposited = total_ecom();
    eosio::asset total_wax(0, wax_symbol);
    rewards_balance_table rewards( _self, _self.value );
	
	//used to check so rewards will not be distributed for insignificant amounts
	bool has_sufficient_balance = false;
	for (const auto& row : rewards) {
		total_wax += row.wax_balance;
		if (row.wax_balance.amount > 100) {
			has_sufficient_balance = true;
		}
	}
	check(has_sufficient_balance, "Not enough rewards accumulated");

    for (const auto& row : rewards) {
        total_wax += row.wax_balance;
    };

    // Iterate through the table and add wax balance to each entry
    auto itr = balances.begin();
    while (itr != balances.end()) {
        balances.modify( itr, same_payer, [&](auto &row) {
            uint64_t reward_wax_amount = (row.ecom_balance.amount * total_wax.amount) / total_ecom().amount;
            row.wax_balance += eosio::asset(reward_wax_amount, wax_symbol);
        });
        itr++;
    }
		auto reward_itr = rewards.find( _self.value );
				rewards.modify( reward_itr, same_payer, [&](auto &row) {
				row.wax_balance = asset(0, wax_symbol);
			});
}


[[eosio::action]]
void eoscommerce3::addrewards(asset quantity, name nft_owner) {
	require_auth(_self); // Require authorization of the contract owner
	int64_t pool_reward_amount = (quantity.amount * 12) / 100;
    int64_t user_reward_amount = (quantity.amount * 88) / 100;
	asset pool_reward(pool_reward_amount, wax_symbol);
	asset user_reward(user_reward_amount, wax_symbol);
  	
  	rewards_balance_table rewards(_self, _self.value);
		auto itr = rewards.find( _self.value );
	if( itr != rewards.end() ){
				rewards.modify( itr, same_payer, [&](auto &row) {
				row.wax_balance += pool_reward;
			});
		} else {
			rewards.emplace( _self, [&](auto &row) {
				row.user = _self.value;
				row.wax_balance = pool_reward;
			});

		}

	account_balance_table balance( _self, _self.value );
		auto itr_users = balance.find( nft_owner.value );

		if( itr_users != balance.end() ){
			//modify this user's balance
			balance.modify( itr_users, same_payer, [&](auto &row) {
				row.wax_balance += user_reward;
			});
		} else {
			balance.emplace( _self, [&](auto &row) {

				row.user = nft_owner.value;
        		row.username = nft_owner;
				row.ecom_balance = asset(0, ecom_symbol);
				row.wax_balance = user_reward;

			});

		}

}


[[eosio::on_notify("eoscommtoken::transfer")]]
void eoscommerce3::on_transfer(name from, name to, eosio::asset quantity, std::string memo) {
		const eosio::name REAL_CONTRACT = "eoscommtoken"_n;
		const eosio::name ORIGINAL_CONTRACT = get_first_receiver();	

		

		check( quantity.amount >= 10000, "Quanity must be greater than 1 ECOM" );
		check( REAL_CONTRACT == ORIGINAL_CONTRACT, "You tryna get over on us, bro?" );

		if( from == _self || to != _self ){ return; }

		check( quantity.symbol == ecom_symbol, "Symbol is not what we were expecting" );

		//emplace info into table
		account_balance_table balance( _self, _self.value );
		auto itr = balance.find( from.value );

		if( itr != balance.end() ){
			//modify this user's balance
			balance.modify( itr, same_payer, [&](auto &row) {
				row.ecom_balance += quantity;
			});
		} else {
			balance.emplace( _self, [&](auto &row) {

				row.user = from.value;
        		row.username = from;
				row.ecom_balance = quantity;
				row.wax_balance = asset(0, wax_symbol);

			});

		}

		// action(permission_level{to, "active"_n}, 
		// "eoscommtoken"_n, "transfer"_n, 
		// std::make_tuple(eosio::name("eoscommerce3"), from, asset(100000, wax_symbol), std::string("We just reacted son"))).send();

        
}
