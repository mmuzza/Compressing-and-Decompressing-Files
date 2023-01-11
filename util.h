//
// STARTER CODE: util.h
// Author: Muhammad Muzzammil
// System: Replit & Zybooks
// Date: 11/28/2022

#pragma once


#include <iostream>
#include <string>
using namespace std;
#include "hashmap.h"
#include "bitstream.h"
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <queue>

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

struct compare
{
    bool operator()(const HuffmanNode *lhs,
        const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};


void freeTree(HuffmanNode* node) {
    
  if(node == nullptr){
    return;
  }
  else{
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
  }  
}


void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {

    int value;

    // if isFile is true, we read the file
    // And add each character and its # of occurence into the map
    if(isFile){
      char character;
      fstream ifstream(filename, fstream::in);
      while (ifstream >> noskipws >> character) {
        // if key already exists, we will increment it
        if(map.containsKey(character)){ 
          value = map.get(character);
          value++;
          map.put(character, value);
        }
          
        else{ // if key does not exist, we asssign it value of 1
          map.put(character, 1);
        }
      }
    }

    // if isFile is false, we DON'T read the file
    // And add each character and its # of occurence from the string itself into the map
    else{
      for(int i = 0; i < filename.size(); i++){ // looping through each chartacter

        // if key already exists, we will increment it
        if(map.containsKey(filename[i])){
          value = map.get(filename[i]);
          value++;
          map.put(filename[i], value);
        }
          
        else{ // if key does not exist, we asssign it value of 1
          map.put(filename[i], 1);
        }
      }
    }

    // These two need to be added separately
    map.put(PSEUDO_EOF, 1); // represents End Of File
}


HuffmanNode* buildEncodingTree(hashmapF &map) {

  priority_queue<HuffmanNode*> basic_pq;
  
    priority_queue<
    HuffmanNode*,
    vector<HuffmanNode*>,
    compare> pq;

    vector<int> keys;
    keys = map.keys();


    // This will prioritize the queue from lowest to highest
    for(int i=0; i<map.size(); i++){
    
      HuffmanNode *tmp = new HuffmanNode;
      tmp->character = keys[i];
      tmp->count = map.get(keys[i]);
      tmp->one = nullptr;
      tmp->zero = nullptr;

      pq.push(tmp);
    }


  // Now we make the tree
  // We will grab 2 nodes from the top of the queue
  // Make a new node that will be not a character
  // That queue will hold the value of the sum of 2 nodes that we took from stack
  // We will repeat this process until there is only 1 node left in the queue
    while(pq.size() > 1){

      HuffmanNode *tmp1 = pq.top();
      pq.pop();
      HuffmanNode *tmp2 = pq.top();
      pq.pop();
      
      HuffmanNode *N_A_C = new HuffmanNode;
      N_A_C->character = NOT_A_CHAR;
      N_A_C->count = tmp1->count + tmp2->count;
      N_A_C->zero = tmp1;
      N_A_C->one = tmp2;

      pq.push(N_A_C);
    }

    return pq.top();
}


void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str) {

  
    if(node == nullptr)
    {
      return;
    }
    else{

      str+= "0"; // We are about to go left
      
      _buildEncodingMap(node->zero, encodingMap, str); // recursive call
      str.pop_back(); // going back to previous node so deleting the 0

      // insert into the map if it is a character
      if(node->character != NOT_A_CHAR){
        encodingMap[node->character] = str;
      }

      str+= "1"; // We are about to go right
      
      _buildEncodingMap(node->one, encodingMap, str); // recursive call
      str.pop_back(); // going back to previous node so deleting the 1
    }
    
}


hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
  
    string binary;

    _buildEncodingMap(tree, encodingMap, binary);
    
    return encodingMap;
}


string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    
    char character;
    string binary; // will hold the binary we get from the tree for each character
  
    while (input >> noskipws >> character) {
      binary += encodingMap[character]; // assigning the binary(value in map) for the character
      size += encodingMap[character].size();
    }

    binary += encodingMap[PSEUDO_EOF]; // this should be added to the end for later use
    size += encodingMap[PSEUDO_EOF].size();


  // put the string in output.writeBit for decode function to read from ifbitsream &input
  if(makeFile){
    for(int i = 0; i < size; i++){
      if(binary[i] == '0'){
        output.writeBit(0);
      }
      else if (binary[i] == '1'){
        output.writeBit(1);
      }
    }
  }
  
    return binary;
}


string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {

  string decoding;
  HuffmanNode* curr = encodingTree;

    while(!input.eof()){
      int bit = input.readBit();


      // if bit is 0 then we move left
      if(bit == 0){
        curr = curr->zero;

        // if our current node points to null thats the node character we will be saving
        // But we have to check if its character or EOF
        if(curr->zero == nullptr){

          // if the character is End of File the break out of loop (We are done)
          if(curr->character == PSEUDO_EOF){
            break;
          }
          // If not we save the character into the string
          else{
            decoding += char(curr->character);
            curr = encodingTree;
          }
        }  
      }

      // if bit is 1 then we move right
      else if (bit == 1){ // if its a '1' 
        curr = curr->one;

        // if our current node points to null thats the node character we will be saving
        // But we have to check if its character or EOF
        if(curr->one == nullptr){

          // if the character is End of File the break out of loop (We are done)
          if(curr->character == PSEUDO_EOF){
            break;
          }
          // If not we save the character into the string
          else{
            decoding += (char)curr->character;
            curr = encodingTree;
          }
        }
      }
    }

    // writing decode string to the output in bitsream file
    for(int i = 0; i < decoding.size(); i++){
      output.put(decoding[i]);
    }
    
    return decoding;
}


string compress(string filename) {

    // done exactly as told in jumpstart

    hashmapF frequencyMap;
    HuffmanNode* encodingTree = nullptr;
    hashmapE encodingMap;
  
    // reads each character and store in map (character : occurence(frequency))
    buildFrequencyMap(filename, true, frequencyMap);

    // Priority queue and builds the tree
    encodingTree = buildEncodingTree(frequencyMap);

    // stores the binary for each character
    encodingMap = buildEncodingMap(encodingTree);
    
    
    ofbitstream output(filename + ".huf");
    output << frequencyMap;
    ifstream input(filename);

    string encoding;
    int size = 0;
    encoding = encode(input, encodingMap, output, size, true);

    freeTree(encodingTree);
  
    return encoding;
}


string decompress(string filename) {

    // done exactly as told in jumpstart
  
    ifbitstream input(filename);
    int pos = filename.find(".txt.huf"); // finding .txt.huf start posit ion
    filename.erase(pos); // deleting the substring starting at pos index
    ofstream output(filename + "_unc.txt");

    
    hashmapF frequencyMap;
    HuffmanNode* encodingTree;
  
    input >> frequencyMap;
    encodingTree = buildEncodingTree(frequencyMap);
  

    string decoding;
    decoding = decode(input, encodingTree, output);
    output.close();

    freeTree(encodingTree);
    
    return decoding;
}