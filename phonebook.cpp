#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

class PhoneRecord
{
private:
    string name;
    string organisation;
    vector<string> phoneNumbers;

public:
    // Constructor
    PhoneRecord(const string &n, const string &org, const vector<string> &numbers)
        : name(n), organisation(org), phoneNumbers(numbers) {}

    // Getter methods
    string getName() const
    {
        return name;
    }

    string getOrganisation() const
    {
        return organisation;
    }

    vector<string> getPhoneNumbers() const
    {
        return phoneNumbers;
    }
};

class HashTableRecord
{
private:
    int key;
    PhoneRecord *element; // Pointer to PhoneRecord
    HashTableRecord *next;

public:
    // Constructor
    HashTableRecord(int k, PhoneRecord *rec)
        : key(k), element(rec), next(nullptr) {}

    // Getter methods
    int getKey() const
    {
        return key;
    }

    PhoneRecord *getRecord() const
    {
        return element;
    }

    HashTableRecord *getNext() const
    {
        return next;
    }

    void setNext(HashTableRecord *nxt)
    {
        next = nxt;
    }
};

class PhoneBook
{
private:
    static const int HASH_TABLE_SIZE = 263;
    HashTableRecord *hashTable[HASH_TABLE_SIZE];

public:
    // Constructor
    PhoneBook()
    {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i)
        {
            hashTable[i] = nullptr;
        }
    }

    // Function to compute hash value for a given string
    long long computeHash(const string &str)
    {
        const long long p = 1000000007;
        const long long x = 263;
        long long hash = 0;

        for (int i = str.length() - 1; i >= 0; --i)
        {
            hash = (hash * x + str[i]) % p;
        }

        return hash % HASH_TABLE_SIZE;
    }

    // Function to add a contact to the hash table
    void addContact(const PhoneRecord *record)
    {
        string name = record->getName();
        istringstream iss(name);

        vector<long long> wordHashes;
        string word;
        long long hash;

        while (getline(iss, word, ' '))
        {
            hash = computeHash(word);
            wordHashes.push_back(hash);
        }

        // Adding the record to the hash table using separate chaining
        for (int i = 0; i < wordHashes.size(); i++)
        {
            HashTableRecord *newRecord = new HashTableRecord(wordHashes[i], (PhoneRecord *)record);
            newRecord->setNext(hashTable[wordHashes[i]]);
            hashTable[wordHashes[i]] = newRecord;
        }
    }

    // Function to delete a contact from the hash table
    bool deleteContact(const string *searchName)
    {
        vector<PhoneRecord *> matchingContacts = fetchContacts(searchName);

        if (matchingContacts.empty())
        {
            return false;
        }

        PhoneRecord *selectedContact = matchingContacts[0];

        vector<long long> nameHashes;
        istringstream iss(selectedContact->getName());
        string word;

        while (getline(iss, word, ' '))
        {
            long long key = computeHash(word);
            nameHashes.push_back(key);
        }

        bool contactDeleted = false;

        // Deleting the contact from the hash table
        for (long long hash : nameHashes)
        {
            int index = hash % HASH_TABLE_SIZE;
            HashTableRecord *current = hashTable[index];
            HashTableRecord *prev = nullptr;

            while (current != nullptr)
            {
                if (current->getRecord() == selectedContact)
                {
                    if (prev == nullptr)
                    {
                        hashTable[index] = current->getNext();
                    }
                    else
                    {
                        prev->setNext(current->getNext());
                    }

                    delete current;
                    contactDeleted = true;
                    break;
                }
                prev = current;
                current = current->getNext();
            }
        }
        return contactDeleted;
    }

    // Function to fetch contacts based on a search query
    vector<PhoneRecord *> fetchContacts(const string *query)
    {
        vector<PhoneRecord *> result;
        map<PhoneRecord *, int> recordCount;

        istringstream iss(*query);
        string word;
        long long key;

        // Counting the occurrences of each record in the hash table
        while (getline(iss, word, ' '))
        {
            key = computeHash(word);
            int index = key % HASH_TABLE_SIZE;
            HashTableRecord *current = hashTable[index];

            while (current != nullptr)
            {
                PhoneRecord *record = current->getRecord();
                recordCount[record]++;
                current = current->getNext();
            }
        }

        // Counting sort part to maintain O(n)
        int maxCount = 0;
        for (const auto &entry : recordCount)
        {
            maxCount = max(maxCount, entry.second);
        }

        vector<vector<PhoneRecord *>> countSort(maxCount + 1);

        for (const auto &entry : recordCount)
        {
            countSort[entry.second].push_back(entry.first);
        }

        // Extracting the sorted PhoneRecord objects into the result vector
        for (int i = maxCount; i >= 0; i--)
        {
            for (PhoneRecord *record : countSort[i])
            {
                result.push_back(record);
            }
        }

        return result;
    }

    // Function to count records pointing to a contact
    int countRecordsPointingTo(const PhoneRecord *record)
    {
        const

        string Name = record->getName();
        istringstream iss(Name);

        string word;
        int count = 0;
        long long key;

        while (getline(iss, word, ' '))
        {
            key = computeHash(word);
            int index = key % HASH_TABLE_SIZE;
            HashTableRecord *current = hashTable[index];

            while (current != NULL)
            {
                if (current->getRecord() == record)
                {
                    count++;
                }
                current = current->getNext();
            }
        }
        return count;
    }

    // Function to read CSV data if provided
    void readRecordsFromFile(const string &filename)
    {
        ifstream file(filename);

        string line;
        while (getline(file, line))
        {

            line.erase(line.end() - 1);
            istringstream iss(line);

            vector<string> tokens;
            string token;

            while (getline(iss, token, ','))
            {
                tokens.push_back(token);
            }

            string name = tokens[0];
            string organisation = tokens[tokens.size() - 1];
            vector<string> phonenumbers;
            for (int i = 1; i <= tokens.size() - 2; i++)
            {
                phonenumbers.push_back(tokens[i]);
            }

            PhoneRecord *record = new PhoneRecord(name, organisation, phonenumbers);
            addContact(record);
        }
        file.close();
    }

    // Destructor
    ~PhoneBook(){
        
    }
};

int main() {
// -------------------------------------------------------- WORKING OF PHONEBOOK -----------------------------------------------------------------------

    // Create a PhoneBook instance
    PhoneBook phoneBook;

    // Add a new contact
    vector<string> newContactNumbers = {"1234567890", "9876543210"};
    PhoneRecord newContact("John Doe", "XYZ Corp", newContactNumbers);
    phoneBook.addContact(&newContact);

    // Fetch contacts based on a query
    string searchQuery = "John";
    vector<PhoneRecord*> searchResults = phoneBook.fetchContacts(&searchQuery);

    // Display the search results
    cout << "Search Results:" << endl;
    for (const auto& result : searchResults) {
        cout << "Name: " << result->getName() << ", Organization: " << result->getOrganisation() << endl;

        cout << "Phonenumber(s): ";
        for (auto number : result->getPhoneNumbers()){
            cout << number << ", ";
        }
        cout<<endl;
    }

    // Count records pointing to a contact
    int recordsCount = phoneBook.countRecordsPointingTo(&newContact);
    cout << "Number of records pointing to the new contact: " << recordsCount << endl;

    // Delete a contact
    bool contactDeleted = phoneBook.deleteContact(&searchQuery);

    if (contactDeleted) {
        cout << "Contact deleted successfully." << endl;
    } else {
        cout << "Contact not found or couldn't be deleted." << endl;
    }

    return 0;
}