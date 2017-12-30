#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cassert>

//Find all occurrences of a given collection of patterns in a string using Suffix Array

const int NumLetters=5;

using namespace std;

//first, build suffix array:

int letterToIndex (char letter)
{
    switch (letter)
    {
        case 'A': return 1; break;
        case 'C': return 2; break;
        case 'G': return 3; break;
        case 'T': return 4; break;
        case '$': return 0; break;
        default: assert (false); return -1;
    }
}

vector<long> SortCharacters(const string& s){
    vector<long> order(s.size());
    vector<int> count(NumLetters,0);
    int c;
    
    //counting sort
    for (long i=0;i<s.size();i++){
        count[letterToIndex(s[i])]++;
    }
    
    for (int j=1;j<NumLetters;j++){
        count[j]+=count[j-1];
    }
    
    for (long i=(long)s.size()-1;i>=0;i--){
        c=letterToIndex(s[i]);
        count[c]--;
        order[count[c]]=i;
    }
    
    return order;
}

vector<long> ComputeCharClasses(const string& s, const vector<long>& order){
    vector<long> classs(s.size());
    
    classs[order[0]]=0;
    for (long i=1;i<s.size();i++){
        if (s[order[i]]!=s[order[i-1]]) classs[order[i]]=classs[order[i-1]]+1;
        else classs[order[i]]=classs[order[i-1]];
    }
    
    return classs;
}

vector<long> SortDoubled(const string& s, const long L, vector<long>& order, const vector<long>& classs){
    
    vector<int> count(s.size(),0);
    vector<long> neworder(s.size());
    long start,cl;
    
    for (long i=0;i<s.size();i++){
        count[classs[i]]++;
    }
    
    for (long i=1;i<s.size();i++){
        count[i]+=count[i-1];
    }
    
    //sorting be the 1st half
    for (long i=(long)s.size()-1;i>=0;i--){
        //reversed order
        start=(order[i]-L+s.size())%(s.size());
        cl=classs[start];
        count[cl]=count[cl]-1;
        neworder[count[cl]]=start;
    }
    
    return neworder;
}

vector<long> UpdateClasses(vector<long>& order, const vector<long>& classs, const long L){
    
    long n=order.size();
    
    vector<long> newclass(n);
    
    long cur,prev,mid,midprev;
    
    newclass[order[0]]=0;
    for (int i=1;i<n;i++){
        cur=order[i];
        prev=order[i-1];
        mid=(cur+L)%n;
        midprev=(prev+L)%n;
        if (classs[cur]!=classs[prev] || classs[mid]!=classs[midprev]) newclass[cur]=newclass[prev]+1;
        else newclass[cur]=newclass[prev];
    }
    
    return newclass;
}


vector<long> BuildSuffixArray(const string& text) {
    
    vector<long> order=SortCharacters(text);
    
    vector<long> classs=ComputeCharClasses(text,order);
    
    long L=1;
    
    while (L<text.size()){
        order=SortDoubled(text,L,order,classs);
        classs=UpdateClasses(order,classs,L);
        L=2*L;
    }
    
    return order;
}

//Now match

int Compare(const string& pattern, const string& text, const long& pos) {
    
    int i=0;
    
    while (i<pattern.size() && (i+pos)<text.size() && letterToIndex(pattern[i])==letterToIndex(text[i+pos])) i++;
    
    if (i==pattern.size()) return 2; //it's a match!
    else if ((i+pos)==text.size()) return 1; //pattern > suf Array $ - this can not happen?
    else if (letterToIndex(pattern[i])>letterToIndex(text[i+pos])) return 1; //pattern > suf Array
    else return 0; //pattern < suf Array
}

vector<long> FindOccurrences(const string& pattern, const string& text, const vector<long>& suffix_array) {
    vector<long> result;
    
    long minIndex=0;
    long maxIndex=text.size();
    long midIndex;
    
    while (minIndex<maxIndex){
        midIndex=(minIndex+maxIndex)/2;
        long pos=suffix_array[midIndex];
        if (Compare(pattern,text,pos)==1) minIndex=midIndex+1;
        else maxIndex=midIndex;
    }
    
    long start=minIndex;
    maxIndex=text.size();
    
    while (minIndex<maxIndex){
        midIndex=(minIndex+maxIndex)/2;
        long pos=suffix_array[midIndex];
        if (Compare(pattern,text,pos)==0) maxIndex=midIndex;
        else minIndex=midIndex+1;
    }
    
    long end=maxIndex;
    
//    while ((start<text.size()-1) && start<=end){
        while (start<end){
        result.push_back(suffix_array[start]);
        start++;
    }

    return result;
}

int main() {
    char buffer[100001];
    scanf("%s", buffer);
    string text = buffer;
    text += '$';
    vector<long> suffix_array = BuildSuffixArray(text);
    int pattern_count;
    scanf("%d", &pattern_count);
    vector<bool> occurs(text.length(), false);
    for (int pattern_index = 0; pattern_index < pattern_count; ++pattern_index) {
        scanf("%s", buffer);
        string pattern = buffer;
        vector<long> occurrences = FindOccurrences(pattern, text, suffix_array);
        for (int j = 0; j < occurrences.size(); ++j) {
            occurs[occurrences[j]] = true;
        }
    }
    for (int i = 0; i < occurs.size(); ++i) {
        if (occurs[i]) {
            printf("%d ", i);
        }
    }
    printf("\n");
    return 0;
}
