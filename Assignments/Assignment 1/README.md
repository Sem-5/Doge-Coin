## TO DO

1. If longer chain created before block generated, remove generate_block event queued for the particular user. Modify `env.cpp`, `receive_block` function.
2. `update_balance` in `user.cpp`. When longer chain generated, need to access up to lca and back to new block and modify balance
3. `choose_transaction` in `user.cpp` to select a random subset of transactions from its transaction pool.

## IMPORTANT NOTE
HEAVY TESTING NEEDED! THERE WERE TOO MANY THINGS TO KEEP TRACK OF AND I HV MP FUCKED HALF OF THE THINGS IN TRYING TO ADD MORE AND MORE CODE. SYNTAX ERRORS, LOGICAL ERRORS, EVERYTHING WILL BE THERE. TEST THIS SHIT PROPERLY. 

Make a `main.cpp` and call simulator, no file format rn, just give inputs in any way u like. Add some loggings here and there and check functionality.