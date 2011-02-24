/** \file
 *  This C header file was generated by $ANTLR version 3.1.3 Mar 17, 2009 19:23:44
 *
 *     -  From the grammar source file : .//LexWhenPred.g
 *     -                            On : 2011-02-24 12:24:17
 *     -                for the parser : LexWhenPredParserParser *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The parser LexWhenPredParser has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pLexWhenPredParser, which is returned from a call to LexWhenPredParserNew().
 *
 * The methods in pLexWhenPredParser are  as follows:
 *
 *  - LexWhenPredParser_program_return      pLexWhenPredParser->program(pLexWhenPredParser)
 *  - LexWhenPredParser_sourceElements_return      pLexWhenPredParser->sourceElements(pLexWhenPredParser)
 *  - LexWhenPredParser_sourceElement_return      pLexWhenPredParser->sourceElement(pLexWhenPredParser)
 *  - LexWhenPredParser_statement_return      pLexWhenPredParser->statement(pLexWhenPredParser)
 *  - LexWhenPredParser_literal_return      pLexWhenPredParser->literal(pLexWhenPredParser)
 *  - LexWhenPredParser_separator_return      pLexWhenPredParser->separator(pLexWhenPredParser)
 * 
 * 
 * 
 * 
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_LexWhenPredParser_H
#define _LexWhenPredParser_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct LexWhenPredParser_Ctx_struct LexWhenPredParser, * pLexWhenPredParser;



  #include <stdlib.h>;
  #include <stdio.h>;
  #include "../emerson/Util.h";


#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/* ========================
 * BACKTRACKING IS ENABLED
 * ========================
 */
typedef struct LexWhenPredParser_program_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    LexWhenPredParser_program_return;

typedef struct LexWhenPredParser_sourceElements_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    LexWhenPredParser_sourceElements_return;

typedef struct LexWhenPredParser_sourceElement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    LexWhenPredParser_sourceElement_return;

typedef struct LexWhenPredParser_statement_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    LexWhenPredParser_statement_return;

typedef struct LexWhenPredParser_literal_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    LexWhenPredParser_literal_return;

typedef struct LexWhenPredParser_separator_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    LexWhenPredParser_separator_return;



/** Context tracking structure for LexWhenPredParser
 */
struct LexWhenPredParser_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_PARSER   pParser;


     LexWhenPredParser_program_return (*program)	(struct LexWhenPredParser_Ctx_struct * ctx);
     LexWhenPredParser_sourceElements_return (*sourceElements)	(struct LexWhenPredParser_Ctx_struct * ctx);
     LexWhenPredParser_sourceElement_return (*sourceElement)	(struct LexWhenPredParser_Ctx_struct * ctx);
     LexWhenPredParser_statement_return (*statement)	(struct LexWhenPredParser_Ctx_struct * ctx);
     LexWhenPredParser_literal_return (*literal)	(struct LexWhenPredParser_Ctx_struct * ctx);
     LexWhenPredParser_separator_return (*separator)	(struct LexWhenPredParser_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred7_LexWhenPred)	(struct LexWhenPredParser_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred8_LexWhenPred)	(struct LexWhenPredParser_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred9_LexWhenPred)	(struct LexWhenPredParser_Ctx_struct * ctx);
     ANTLR3_BOOLEAN (*synpred10_LexWhenPred)	(struct LexWhenPredParser_Ctx_struct * ctx);
    // Delegated rules
    const char * (*getGrammarFileName)();
    void	    (*free)   (struct LexWhenPredParser_Ctx_struct * ctx);
    /* @headerFile.members() */
    pANTLR3_BASE_TREE_ADAPTOR	adaptor;
    pANTLR3_VECTOR_FACTORY		vectors;
    /* End @headerFile.members() */
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pLexWhenPredParser LexWhenPredParserNew         (pANTLR3_COMMON_TOKEN_STREAM instream);
ANTLR3_API pLexWhenPredParser LexWhenPredParserNewSSD      (pANTLR3_COMMON_TOKEN_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the parser will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define HexEscapeSequence      14
#define LineComment      31
#define DecimalDigit      19
#define EOF      -1
#define HexDigit      20
#define Identifier      7
#define SingleStringCharacter      11
#define PROG      5
#define T__51      51
#define Comment      30
#define SingleEscapeCharacter      16
#define UnicodeLetter      26
#define ExponentPart      23
#define EscapeCharacter      18
#define WhiteSpace      32
#define T__50      50
#define IdentifierPart      25
#define UnicodeCombiningMark      29
#define T__42      42
#define LTERM      4
#define T__43      43
#define UnicodeDigit      27
#define T__40      40
#define T__41      41
#define T__46      46
#define T__47      47
#define NumericLiteral      9
#define T__44      44
#define T__45      45
#define T__48      48
#define T__49      49
#define UnicodeEscapeSequence      15
#define WHEN_PRED_BLOCK      6
#define IdentifierStart      24
#define DoubleStringCharacter      10
#define DecimalLiteral      21
#define StringLiteral      8
#define T__33      33
#define T__34      34
#define T__35      35
#define T__36      36
#define T__37      37
#define T__38      38
#define T__39      39
#define HexIntegerLiteral      22
#define NonEscapeCharacter      17
#define CharacterEscapeSequence      13
#define EscapeSequence      12
#define UnicodeConnectorPunctuation      28
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for LexWhenPredParser
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
