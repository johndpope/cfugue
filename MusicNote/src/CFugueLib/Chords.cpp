
#include "stdafx.h"

#include "Chords.h"
#include <algorithm>

namespace CFugue
{
    ///<brief>Standard Chord Definitions</brief>
    /// Each entry in the chord definition holds the chord name, followed by the half-step intervals.
    static const ChordDef DefChordDefinitions[] =  
    {
        {_T("MAJ"),         {4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0},      2},
        {_T("MIN"),         {3,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0},      2},
        {_T("AUG"),         {4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0},      2},
        {_T("DIM"),         {3,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0},      2},
        {_T("DOM7"),        {4,7,10,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MAJ7"),        {4,7,11,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MIN7"),        {3,7,10,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("SUS4"),        {5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0},      2},
        {_T("SUS2"),        {2,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0},      2},
        {_T("MAJ6"),        {4,7,9,0,0,0,0,0,0,0,0,0,0,0,0,0},      3},
        {_T("MIN6"),        {3,7,9,0,0,0,0,0,0,0,0,0,0,0,0,0},      3},
        {_T("DOM9"),        {4,7,10,14,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("MAJ9"),        {4,7,11,14,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("MIN9"),        {3,7,10,14,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DIM7"),        {3,6,9,0,0,0,0,0,0,0,0,0,0,0,0,0},      3},
        {_T("ADD9"),        {4,7,14,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MIN11"),       {7,10,14,15,17,0,0,0,0,0,0,0,0,0,0,0},  5},
        {_T("DOM11"),       {7,10,14,17,0,0,0,0,0,0,0,0,0,0,0,0},   4},
        {_T("DOM13"),       {7,10,14,16,21,0,0,0,0,0,0,0,0,0,0,0},  5},
        {_T("MIN13"),       {7,10,14,15,21,0,0,0,0,0,0,0,0,0,0,0},  5},
        {_T("MAJ13"),       {7,11,14,16,21,0,0,0,0,0,0,0,0,0,0,0},  5},
        {_T("DOM7_5"),      {4,6,10,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("DOM7<5"),      {4,6,10,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("DOM75"),       {4,8,10,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("DOM7>5"),      {4,8,10,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MAJ7_5"),      {4,6,11,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MAJ7<5"),      {4,6,11,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MAJ75"),       {4,8,11,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MAJ7>5"),      {4,8,11,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("MINMAJ7"),     {3,7,11,0,0,0,0,0,0,0,0,0,0,0,0,0},     3},
        {_T("DOM7_5_9"),    {4,6,10,13,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM7<5<9"),    {4,6,10,13,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM7_59"),     {4,6,10,15,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM7<5>9"),    {4,6,10,15,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM75_9"),     {4,8,10,13,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM7>5<9"),    {4,8,10,13,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM759"),      {4,8,10,15,0,0,0,0,0,0,0,0,0,0,0,0},    4},
        {_T("DOM7>5>9"),    {4,8,10,15,0,0,0,0,0,0,0,0,0,0,0,0},    4},
	};

#ifndef _countof
	// helper function to find the size of array
	template < typename T, size_t N >
	inline size_t _countof(T(&arr)[N])	{ return std::extent< T[N] >::value; }
#endif

    // Return if the first element is greater than the second
    template<typename T>
    bool phRepComparator(const T* input1, const T* input2)
    {
        return _tcslen(input1->szChordName) > _tcslen(input2->szChordName);
    }

    template<typename T>
    void LoadVector(std::map<TCHAR, std::vector<const T*> >& vecArray, const T* inputArr, int nInputSize)
    {
        // For each entry in the definition
        for(int i=0; i < nInputSize; ++i)
        {
            const T& Obj = inputArr[i];
            // Find the First character
            const TCHAR chIndex = Obj.szChordName[0];
            // Store the definition, indexed at its first character 
            vecArray[chIndex].push_back(inputArr+i);
        }

		typedef typename std::map<TCHAR, std::vector<const T*> >::value_type VecIterTypeT;

        // Sort each vector such that longer strings come first
		std::for_each(vecArray.begin(), vecArray.end(), [](VecIterTypeT& iter)
		{
			std::vector<const T*>& vec = iter.second;
			std::sort(vec.begin(), vec.end(), phRepComparator<T>);
		});
    }

    // Checks if the given prefix string is complete present in the string.
    // Return value would be same as the length of the prefix string, if successful.
    // Return value would be zero, in case the prefix string is not present completely.
    inline unsigned int IsPrefixMatching(const TCHAR* sz, const TCHAR* pfx)
    {
        unsigned int nMatched = 0;
        
        while(*sz && *pfx && *sz == *pfx) 
        {
            sz++;
            pfx++;
            nMatched++;
        }
        
        return (*pfx == _T('\0')) ? nMatched : 0;
    }

    // Searches the vectors to find the best prefix that matches the sequence of
    // characters pointed by sz.
    // @param retVal Holds the object whose strint completely matched the prefix of sz
    // @return the number of characters correctly matched (should be the same as the length of retVal)
    template<typename T>
    inline unsigned int ExtractMatchingObject(const std::map<TCHAR, std::vector<const T*> >& vecArray, const TCHAR* sz, T* retVal)
    {
        const TCHAR chIndex =  sz[0];

		const auto iterVecObjects = vecArray.find(chIndex);

		if(iterVecObjects != vecArray.end())
		{
			const std::vector<const T*>& vecObjects = iterVecObjects->second;

			unsigned int nMatched = 0;

			for(size_t i=0, nMax = vecObjects.size(); i <  nMax; ++i)
			{
				*retVal = *vecObjects[i];

				if((nMatched = IsPrefixMatching(sz, retVal->szChordName)) > 0)
					return nMatched;
			}
		}

        return 0;
    }

    Chords::Chords()
    {
        LoadDefinitions();
    }

    Chords::Chords(const ChordDef* pChords, int nSize)
    {
        LoadDefinitions(pChords, nSize);
    }
       
    void Chords::LoadDefinitions(const ChordDef* pChords /*=NULL*/, int nSize /*=0*/)
    {
        m_Definitions.clear();
        if(pChords == NULL || nSize == 0)
        {   
            pChords = DefChordDefinitions;
            nSize = _countof(DefChordDefinitions);
        }
        LoadVector(this->m_Definitions, pChords, nSize);
    }
        
    void Chords::AddDefinitions(const ChordDef* pChords, int nSize)
    {
        // Load the input Arrays into internal structures        
        if(pChords != NULL &&  nSize != 0)            
            LoadVector(this->m_Definitions, pChords, nSize);
    }

    unsigned int Chords::ExtractMatchingChord(const TCHAR* szToken, ChordDef* retVal) const
    {
        return ExtractMatchingObject(this->m_Definitions, szToken, retVal);
    }

	unsigned int Chords::GetDefaultMatchingChord(const TCHAR* szToken, ChordDef* retVal)
	{
		static Chords staticObj; // Loads the default chord definitions
		return staticObj.ExtractMatchingChord(szToken, retVal);
	}

} // namespace CFugue