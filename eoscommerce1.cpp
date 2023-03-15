#include "eoscommerce1.hpp"


void eoscommerce1::check_transfer(name contract, asset quantity) {
  const name REAL_CONTRACT = "eoscommtoken"_n;
  check(contract == REAL_CONTRACT, "Invalid token contract");

  const symbol ecom_symbol = symbol("ECOM", 4);
  check(quantity.symbol == ecom_symbol, "Invalid symbol in quantity");
  check(quantity.amount >= 10000, "Quantity must be greater than 1 ECOM");
}


[[eosio::action]]
void eoscommerce1::addbalance(name account, asset quantity) {
  require_auth(account);

  // Check the token contract and quantity
  check_transfer(get_first_receiver(), quantity);

  account_balance_table balances(_self, _self.value);
  auto balance_itr = balances.find(account.value);

  if (balance_itr == balances.end()) {
    // If the account is not in the table, add it with the transferred quantity
    balances.emplace(account, [&](auto& row) {
      row.account = account;
      row.balance = quantity;
    });
  } else {
    // If the account is in the table, add the transferred quantity to the existing balance
    balances.modify(balance_itr, account, [&](auto& row) {
      row.balance += quantity;
    });
  }
}

[[eosio::on_notify("eoscommtoken::transfer")]]
void eoscommerce1::on_transfer(name from, name to, asset quantity, std::string memo) {
  const eosio::name REAL_CONTRACT = "eoscommtoken"_n;
	const eosio::name ORIGINAL_CONTRACT = get_first_receiver();
  const std::string_view ecom_string{"ECOM"};
	const uint8_t ecom_decimals = 4;

		const symbol ecom_symbol(ecom_string, ecom_decimals);

  check_transfer(get_first_receiver(), quantity);
  account_balance_table balances(_self, _self.value);
  auto balance_itr = balances.find(from.value);

  if (balance_itr == balances.end()) {
    // If the account is not in the table, add it with the transferred quantity
    balances.emplace(from, [&](auto& row) {
      row.account = account;
      row.balance = quantity;
    });
  } else {
    // If the account is in the table, add the transferred quantity to the existing balance
    balances.modify(balance_itr, from, [&](auto& row) {
      row.balance += quantity;
    });
  }
}
