#include "EmersonUtil.h"
#include <antlr3.h>

extern pANTLR3_UINT8  EmersonParserTokenNames[];





int emerson_isImaginaryToken(pANTLR3_COMMON_TOKEN token)
{
  return 0;
}




void emerson_printRewriteStream(pANTLR3_REWRITE_RULE_TOKEN_STREAM rwStream)
{
  pANTLR3_BASE_TREE tree = rwStream->_next(rwStream);
  while(tree != NULL)
		{
		  pANTLR3_COMMON_TOKEN token = tree->getToken(tree);
		  if( !(emerson_isImaginaryToken(token))) 
				{
				  printf(token->getText(token)->chars);
		  }

    tree = rwStream->_next(rwStream);
		}
  return;
}

pANTLR3_STRING emerson_printAST(pANTLR3_BASE_TREE tree)
{
  pANTLR3_STRING  string;
  ANTLR3_UINT32   i;
  ANTLR3_UINT32   n;
	 pANTLR3_BASE_TREE   t;


		if	(tree->children == NULL || tree->children->size(tree->children) == 0)
  {
    return	tree->toString(tree);
  }
  
		// THis is how you get a new string. The string is blank

  string	= tree->strFactory->newRaw(tree->strFactory);

  if	(tree->isNilNode(tree) == ANTLR3_FALSE)
		{
		  string->append8	(string, "(");
				pANTLR3_COMMON_TOKEN token = tree->getToken(tree);
    ANTLR3_UINT32 type = token->type;
				string->append(string, EmersonParserTokenNames[type]);
	   string->append8	(string, " ");
  }

  if	(tree->children != NULL)
		{
		  n = tree->children->size(tree->children);
				for	(i = 0; i < n; i++)
				{   
				  t   = (pANTLR3_BASE_TREE) tree->children->get(tree->children, i);

						if  (i > 0)
						{
						  string->append8(string, " ");
						}
						string->appendS(string, emerson_printAST(t));
					}
			}
   
			if	(tree->isNilNode(tree) == ANTLR3_FALSE)
				{
						string->append8(string,")");
				}

				return  string;

}

void emerson_createTreeMirrorImage2(pANTLR3_BASE_TREE ptr)
{
  
  if(ptr!= NULL && ptr->children != NULL)
  {

				ANTLR3_UINT32 n = ptr->getChildCount(ptr);
				if(n == 1)
				{
				  //emerson_createTreeMirrorImage((pANTLR3_BASE_TREE)(ptr->getChild(ptr, 0)));
				}
				if(n == 2)  // should it be checked
				{
				  pANTLR3_BASE_TREE right = (pANTLR3_BASE_TREE)(ptr->getChild(ptr, 1));
		    //emerson_createTreeMirrorImage( (pANTLR3_BASE_TREE)(ptr->getChild(ptr, 0)));
			   //emerson_createTreeMirrorImage( (pANTLR3_BASE_TREE)(ptr->getChild(ptr, 1)) );
				  ptr->setChild(ptr, 1, ptr->getChild(ptr, 0));
						ptr->setChild(ptr, 0, right);
				}		
		}


}


void emerson_createTreeMirrorImage(pANTLR3_BASE_TREE ptr)
{
  
  if(ptr!= NULL && ptr->children != NULL)
  {

				ANTLR3_UINT32 n = ptr->getChildCount(ptr);
				if(n == 1)
				{
				  emerson_createTreeMirrorImage((pANTLR3_BASE_TREE)(ptr->getChild(ptr, 0)));
				}
				if(n == 2)  // should it be checked
				{
				  pANTLR3_BASE_TREE right = (pANTLR3_BASE_TREE)(ptr->getChild(ptr, 1));
		    emerson_createTreeMirrorImage( (pANTLR3_BASE_TREE)(ptr->getChild(ptr, 0)));
			   emerson_createTreeMirrorImage( (pANTLR3_BASE_TREE)(ptr->getChild(ptr, 1)) );
				  ptr->setChild(ptr, 1, ptr->getChild(ptr, 0));
						ptr->setChild(ptr, 0, right);
				}		
		}


}


#include "EmersonLexer.h"
#include "EmersonParser.h"
#include "EmersonTree.h"

char* emerson_compile(const char* em_script_str)
{

 printf("Trying to compile \n %s\n", em_script_str);
	


 pANTLR3_UINT8 str = (pANTLR3_UINT8)em_script_str; 
 pANTLR3_INPUT_STREAM input = antlr3NewAsciiStringCopyStream(str, strlen(em_script_str), NULL);     
	char* js_str;
	pEmersonLexer lxr;
	pEmersonParser psr;
	pANTLR3_COMMON_TOKEN_STREAM tstream;
	EmersonParser_program_return emersonAST;
	pANTLR3_COMMON_TREE_NODE_STREAM	nodes;
 pEmersonTree treePsr;

	if (input == NULL)
 {
	 fprintf(stderr, "Unable to create input stream");
			exit(ANTLR3_ERR_NOMEM);
 }
 lxr	    = EmersonLexerNew(input);
 if ( lxr == NULL )
 {
			fprintf(stderr, "Unable to create the lexer due to malloc() failure1\n");
			exit(ANTLR3_ERR_NOMEM);
 }
  tstream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

 if (tstream == NULL)
 {
	  fprintf(stderr, "Out of memory trying to allocate token stream\n");
		 exit(ANTLR3_ERR_NOMEM);
 }

 psr	    = EmersonParserNew(tstream);  // CParserNew is generated by ANTLR3

 if (psr == NULL)
 {
   fprintf(stderr, "Out of memory trying to allocate parser\n");
		 exit(ANTLR3_ERR_NOMEM);
 }

 emersonAST = psr->program(psr);
if (psr->pParser->rec->state->errorCount > 0)
	{
		fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", psr->pParser->rec->state->errorCount);

	}
	else
	{


   //printf("Emerson Tree after parsing \n%s\n", emerson_printAST(emersonAST.tree)->chars);
   nodes	= antlr3CommonTreeNodeStreamNewTree(emersonAST.tree, ANTLR3_SIZE_HINT); // sIZE HINT WILL SOON BE DEPRECATED!!
   treePsr	= EmersonTreeNew(nodes);
		 js_str = treePsr->program(treePsr)->chars;
		 nodes   ->free  (nodes);	    nodes	= NULL;
		 treePsr ->free  (treePsr);	    treePsr	= NULL;

 }
 psr	    ->free  (psr);		psr		= NULL;
	tstream ->free  (tstream);	tstream	= NULL;
	lxr	    ->free  (lxr);	    lxr		= NULL;
	input   ->close (input);	input	= NULL;
 
 return js_str;

}
