// SPDX-License-Identifier: MIT

pragma solidity ^0.8.7;

/** 
 * @title CS765 DAPP
 * @dev Implements a DAPP for transfer using joint accounts
 */
 
contract JointAccount {
    
    mapping(uint256 => bool) public graphId;
    mapping(uint256 => string) public userName;
    
    mapping(uint256 => mapping(uint256 => bool)) public adjMap;
    mapping(uint256 => mapping(uint256 => uint64)) public weiMap;
    mapping(uint256 => mapping(uint256 => uint256)) public adjListIdx;
    mapping(uint256 => uint256[]) public adjList;
    
    mapping(uint256 => bool) visited;
    mapping(uint256 => uint256) parent;
    uint256[] queue;
    
    // add a user to transferable list
    function registerUser (uint256 _user_id, string memory _user_name) external {
        require(graphId[_user_id] == false, "Account already exists");
        graphId[_user_id] = true;
        userName[_user_id] = _user_name;
    }
    
    // creates a 1 way account 
    // call with reversed arguments for 2 way creation
    // If account already exists, adds the amount
    function createAcc(uint256 _user_id_1, uint256 _user_id_2, uint64 _amount) external
    {
        require(graphId[_user_id_1], "User 1 is not registered");
        require(graphId[_user_id_2], "User 2 is not registered");
        require(_amount >= 0, "Negative amount is not valid");
        if (adjMap[_user_id_1][_user_id_2] == false)
        {
            adjMap[_user_id_1][_user_id_2] = true;
            adjListIdx[_user_id_1][_user_id_2] = adjList[_user_id_1].length;
            adjList[_user_id_1].push(_user_id_2);
        }
        weiMap[_user_id_1][_user_id_2] += _amount;
    }
    
    // send Amount _amount from _user_id_1 to uid _user_id_2
    // via the shortest possible path
    function sendAmount(uint256 _user_id_1, uint256 _user_id_2, uint64 _amount) external
    {
        require(graphId[_user_id_1], "User 1 is not registered");
        require(graphId[_user_id_2], "User 2 is not registered");
        require(_amount >= 0, "Negative amount is not valid");
        
        
        // BFS on valid edges

        bool success = false;

        parent[_user_id_1] = _user_id_1;
        visited[_user_id_1] = true;
        queue.push(_user_id_1);
        uint256 idx = 0;
        uint256 sz = 1;
        
        while(idx < sz)
        {
            uint256 node = queue[idx];
            if(node == _user_id_2)
            {
                success = true;
                break;
            }
            for (uint256 i = 0; i < adjList[node].length; ++i)
            {
                if (weiMap[node][adjList[node][i]] >= _amount)
                {
                    if(visited[adjList[node][i]] == false)
                    {
                        visited[adjList[node][i]] = true;
                        parent[adjList[node][i]] = node;
                        queue.push(adjList[node][i]);
                        sz++;
                    }
                }
            }
            idx++;
        }
    
        if(success)
        {
            uint256 child = _user_id_2;
            while(child != _user_id_1)
            {
                weiMap[child][parent[child]] += _amount;
                weiMap[parent[child]][child] -= _amount;
                child = parent[child];
            }
        }
        // =============
        
        // reset the state of visited, parent and queue
        while(queue.length != 0)
        {
            uint256 delnode = queue[queue.length-1];
            queue.pop();
            parent[delnode] = 0;
            visited[delnode] = false;
        }
        
        require(success, "No valid path exists to enable transfer");
    }
    
    // close the account held between two people
    // one way closure, call twice for 2 way
    function closeAccount(uint256 _user_id_1, uint256 _user_id_2) external
    {
        require(adjMap[_user_id_1][_user_id_2], "No account exists");
        uint256 lastaddr = adjList[_user_id_1][adjList[_user_id_1].length-1];
        adjList[_user_id_1][adjList[_user_id_1].length-1] = _user_id_2;
        adjList[_user_id_1][adjListIdx[_user_id_1][_user_id_2]] = lastaddr;
        adjList[_user_id_1].pop();
        adjListIdx[_user_id_1][lastaddr] = adjListIdx[_user_id_1][_user_id_2];
        adjListIdx[_user_id_1][_user_id_2] = 0;
        adjMap[_user_id_1][_user_id_2] = false;
        weiMap[_user_id_1][_user_id_2] = 0;
    }
    
}
