/*** term.c ********************************************************************
** 
** This file is part of BibTool.
** It is distributed under the GNU General Public License.
** See the file COPYING for details.
** 
** (c) 2015-2016 Gerd Neugebauer
** 
** Net: gene@gerd-neugebauer.de
** 
******************************************************************************/

#include <bibtool/general.h>
#include <bibtool/error.h>
#include "term.h"

/*****************************************************************************/
/* Internal Programs                                                         */
/*===========================================================================*/

#ifdef __STDC__
#define _ARG(A) A
#else
#define _ARG(A) ()
#endif

 Term new_term _ARG((short int type, Term car, Term cdr ));
 Term new_term_num _ARG((long value));
 Term new_t_string _ARG((short int type, unsigned char* s));
 void free_term _ARG((Term term));
 void print_term _ARG((FILE* file, Term term));
 String tag_id _ARG((int c));

 static void prn_args _ARG((FILE * file, Term term, char* sep,int in));/*    */
 static void prn_term _ARG((FILE * file, Term term, int in));/*              */

/*****************************************************************************/
/* External Programs                                                         */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
** Function:	tag_id()
** Type:	static String
** Purpose:	
**		
** Arguments:
**	c	
** Returns:	
**___________________________________________________			     */
String tag_id(c)			   	   /*                        */
  int c;					   /*                        */
{ static Uchar buffer[2];
						   /*                        */
  switch (c)					   /*                        */
  { case L_STRING:   return (String)"string";	   /*                        */
    case L_FIELD:    return (String)"field";	   /*                        */
    case L_NUMBER:   return (String)"number";	   /*                        */
    case L_FUNCTION: return (String)"function";	   /*                        */
    case L_CONS:     return (String)"cons";	   /*                        */
    case L_GROUP:    return (String)"group";	   /*                        */
    case L_TRUE:     return (String)"true";	   /*                        */
    case L_FALSE:    return (String)"false";	   /*                        */
    case L_QUOTE:    return (String)"'";	   /*                        */
    case L_UMINUS:   return (String)"-";	   /*                        */
    case L_MINUS:    return (String)"-";	   /*                        */
    case L_PLUS:     return (String)"+";	   /*                        */
    case L_TIMES:    return (String)"*";	   /*                        */
    case L_DIV:      return (String)"/";	   /*                        */
    case L_MOD:      return (String)"mod";	   /*                        */
    case L_SET:      return (String)"=";	   /*                        */
    case L_LIKE:     return (String)"like";	   /*                        */
    case L_ILIKE:    return (String)"ilike";	   /*                        */
    case L_EQ:       return (String)"==";	   /*                        */
    case L_NE:       return (String)"!=";	   /*                        */
    case L_GT:       return (String)">";	   /*                        */
    case L_GE:       return (String)">=";	   /*                        */
    case L_LT:       return (String)"<";	   /*                        */
    case L_LE:       return (String)"<=";	   /*                        */
    case L_NOT:      return (String)"not";	   /*                        */
    case L_AND:      return (String)"and";	   /*                        */
    case L_OR:       return (String)"or";	   /*                        */
    case L_IF:       return (String)"if";	   /*                        */
    case L_WHILE:    return (String)"while";	   /*                        */
    case L_WITH:     return (String)"with";	   /*                        */
    case 0:					   /*                        */
    case EOF:        return (String)"end of file"; /*                        */
  }						   /*                        */
  buffer[0] = c;				   /*                        */
  buffer[1] = 0;				   /*                        */
  return buffer;				   /*                        */
}						   /*------------------------*/

/*---------------------------------------------------------------------------*/


 Term term_eof;

 static Term terms = NIL;

/*-----------------------------------------------------------------------------
** Function:	new_term()
** Type:	Term
** Purpose:	Allocate a new term and initialize it.
**		
** Arguments:
**	sym	the symdef
**	car	the car
**	cdr	the cdr
** Returns:	
**___________________________________________________			     */
Term new_term(type, car, cdr)			   /*                        */
  short int type;				   /*                        */
  Term car;					   /*                        */
  Term cdr;					   /*                        */
{ register Term t = terms;			   /*                        */
  						   /*                        */
  if (t) { terms = Car(t); }			   /*                        */
  else						   /*                        */
  { t = (Term)malloc(sizeof(STerm));		   /*                        */
    if (t == NIL) OUT_OF_MEMORY("term");	   /*                        */
  }	   					   /*                        */
  TType(t)     = type;			   	   /*                        */
  Car(t)       = car;			   	   /*                        */
  Cdr(t)       = cdr;			   	   /*                        */
  TRefCount(t) = 1L;				   /*                        */
  return t;					   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	new_term_num()
** Type:	Term
** Purpose:	Allocate a new term and initialize it as number.
**		
** Arguments:
**	n	the numeric value
** Returns:	
**___________________________________________________			     */
Term new_term_num(n)				   /*                        */
  long n;					   /*                        */
{ register Term t = terms;			   /*                        */
  						   /*                        */
  if (t) { terms = Car(t); }			   /*                        */
  else						   /*                        */
  { t = (Term)malloc(sizeof(STerm));		   /*                        */
    if (t == NIL) OUT_OF_MEMORY("term");	   /*                        */
  }	   					   /*                        */
  TType(t)     = L_NUMBER;			   /*                        */
  TNumber(t)   = n;				   /*                        */
  Cdr(t)       = NIL;			   	   /*                        */
  TRefCount(t) = 1L;				   /*                        */
  return t;					   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	new_t_string()
** Type:	Term
** Purpose:	Allocate a new term and initialize it as string type.
**		
** Arguments:
**	sym	the symdef
**	s	the string value, i.e. a symbol
** Returns:	
**___________________________________________________			     */
Term new_t_string(type, s)		   	   /*                        */
  short int type;				   /*                        */
  String s;					   /*                        */
{ register Term t = terms;			   /*                        */
  						   /*                        */
  if (t) { terms = Car(t); }			   /*                        */
  else						   /*                        */
  { t = (Term)malloc(sizeof(STerm));		   /*                        */
    if (t == NIL) OUT_OF_MEMORY("term");	   /*                        */
  }	   					   /*                        */
  TType(t)     = type;			   	   /*                        */
  TString(t)   = s;				   /*                        */
  Cdr(t)       = NIL;			   	   /*                        */
  TRefCount(t) = 1L;				   /*                        */
  return t;					   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	free_term()
** Type:	void
** Purpose:	Free the memory of a term and arrange for reuse.
**		The term odes are linked into the |terms| list to be reused.
**		This happens only for those term nodes which are not locked.
**		
** Arguments:
**	t	
** Returns:	nothing
**___________________________________________________			     */
void free_term(t)				   /*                        */
  register Term t;				   /*                        */
{ Term cdr;					   /*                        */
 						   /*                        */
  if (t == NIL) return;				   /*                        */
 						   /*                        */
  cdr = Cdr(t);					   /*                        */
 						   /*                        */
#ifdef TODO
  switch (TType(t))				   /*                        */
  { case -10:					   /* list type              */
      free_term(Car(t));			   /*                        */
      break;					   /*                        */
    case -11:					   /* number type            */
    case -12:					   /* string type            */
      break;					   /*                        */
    default:					   /* locked                 */
      return;					   /*                        */
  }						   /*                        */
#endif
  Car(t) = terms;				   /*                        */
  terms = t;					   /*                        */
 						   /*                        */
  if (cdr)					   /*                        */
  { free_term(cdr);			   	   /*                        */
    Cdr(t) = NIL;				   /*                        */
  }						   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	prn_quoted()
** Type:	static void
** Purpose:	
**		
** Arguments:
**	file	the output stream
**	s	the string to be printed
** Returns:	nothing
**___________________________________________________			     */
static void prn_quoted(file, s)		   	   /*                        */
  FILE * file;					   /*                        */
  String s;					   /*                        */
{						   /*                        */
  for (; *s; s++)				   /*                        */
  { switch (*s)					   /*                        */
    { case '\n': fputs("\\n", file);  break;	   /*                        */
      case '\r': fputs("\\r", file);  break;	   /*                        */
      case '\t': fputs("\\t", file);  break;	   /*                        */
      case '\b': fputs("\\b", file);  break;	   /*                        */
      case '\f': fputs("\\f", file);  break;	   /*                        */
      case '"':	 fputs("\\\"", file); break;	   /*                        */
      case '\'': fputs("\\'", file);  break;	   /*                        */
      case '\\': fputs("\\\\", file); break;	   /*                        */
      default:	 fputc((char)*s, file);		   /*                        */
    }						   /*                        */
  }						   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	prn_field()
** Type:	static int
** Purpose:	
**		
** Arguments:
**	file	
**	t	
** Returns:	
**___________________________________________________			     */
void prn_field(file, t)		   	   	   /*                        */
  FILE * file;					   /*                        */
  Term t;					   /*                        */
{ int q	 = 0;					   /*                        */
  String s = TString(t);			   /*                        */
  if (*s >= '0' && *s <= '9') {			   /*                        */
    q = 1;					   /*                        */
  } else {					   /*                        */
    for (; *s; s++)				   /*                        */
    { if (!(   (*s >='a' && *s <='z') 		   /*                        */
	    || (*s >='A' && *s <='Z')		   /*                        */
	    || (*s >='0' && *s <='9')		   /*                        */
	    || *s == '@'			   /*                        */
	    || *s == '$'			   /*                        */
	    || *s == '_'			   /*                        */
	    || *s == '.'))			   /*                        */
      { q = 1;					   /*                        */
	break;					   /*                        */
      }						   /*                        */
    }						   /*                        */
  }						   /*                        */
  if (q) fputc('`', file);			   /*                        */
  prn_quoted(file, TString(t));		   	   /*                        */
  if (q) fputc('`', file);			   /*                        */
 						   /*                        */
  if (Cdr(t))					   /*                        */
  { fputs(": ", file);				   /*                        */
    prn_term(file, Cdr(t), 0);			   /*                        */
  }						   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	indent()
** Type:	static void
** Purpose:	
**		
** Arguments:
**	file	
**	s	
**	in	
** Returns:	nothing
**___________________________________________________			     */
static void indent(file, s, in)			   /*                        */
  FILE * file;					   /*                        */
  char * s;					   /*                        */
  int in;					   /*                        */
{ int i;					   /*                        */
  fputs(s, file);				   /*                        */
  for (i = 0; i < in; i++) fputs("  ", file);	   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	prn_args()
** Type:	static void
** Purpose:	
**		
** Arguments:
**	file	
**	term	
**	sep	
**	in	
** Returns:	nothing
**___________________________________________________			     */
static void prn_args(file, term, sep, in)	   /*                        */
  FILE * file;					   /*                        */
  Term term;					   /*                        */
  char * sep;					   /*                        */
  int in;					   /*                        */
{						   /*                        */
  if (term == NIL) return;			   /*                        */
 						   /*                        */
  prn_term(file, Car(term), in);		   /*                        */
  for (term = Cdr(term); term; term = Cdr(term))   /*                        */
  { indent(file, sep, in);			   /*                        */
    prn_term(file, Car(term), in);		   /*                        */
  }						   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	prn_term()
** Type:	void
** Purpose:	Produce a printed representation of a term and write it to
**		the output stream.
** Arguments:
**	file	the output stream
**	t	the term to print
** Returns:	nothing
**___________________________________________________			     */
static void prn_term(file, term, in)		   /*                        */
  FILE * file;					   /*                        */
  Term term;					   /*                        */
  int in;					   /*                        */
{ char * key;					   /*                        */
  if (term == NIL) {				   /*                        */
    fputs("nil", file);				   /*                        */
    return;					   /*                        */
  }						   /*                        */
 						   /*                        */
  switch (TType(term))				   /*                        */
  { case 0:					   /*                        */
    case EOF:					   /*                        */
      return;	   			   	   /*                        */
    case L_STRING:				   /*                        */
      fputc('"', file);	   			   /*                        */
      prn_quoted(file, TString(term));	   	   /*                        */
      fputc('"', file);	   			   /*                        */
      return;					   /*                        */
    case L_BLOCK:				   /*                        */
      fputc('{', file);	   			   /*                        */
      fputs((char*)TString(term), file);	   /*                        */
      fputc('}', file);	   			   /*                        */
      return;					   /*                        */
    case L_FIELD:				   /*                        */
      prn_field(file, term);			   /*                        */
      return;					   /*                        */
    case L_NUMBER:				   /*                        */
      fprintf(file, "%ld", TNumber(term));	   /*                        */
      return;					   /*                        */
    case L_TRUE:				   /*                        */
      fputs("true", file);			   /*                        */
      return;					   /*                        */
    case L_FALSE:				   /*                        */
      fputs("false", file);			   /*                        */
      return;					   /*                        */
    case L_CONS:				   /*                        */
      fputs("[", file);			   	   /*                        */
      prn_args(file, term, " ", 0);		   /*                        */
      fputs("]", file);			   	   /*                        */
      return;					   /*                        */
    case L_GROUP:				   /*                        */
      if (Cdr(term))				   /*                        */
      { indent(file, "{\n", in + 1);		   /*                        */
	prn_args(file, Cdr(term), ";\n", in + 1);  /*                        */
	indent(file, "\n", in);	   		   /*                        */
      } else {					   /*                        */
	fputs("{\n", file);			   /*                        */
      }						   /*                        */
      fputs("}", file);			   	   /*                        */
      return;					   /*                        */
    case L_WHILE:				   /*                        */
      fputs("while (", file);			   /*                        */
      prn_args(file, Cdar(term), "", in + 1);	   /*                        */
      fputs(") ", file);			   /*                        */
      prn_term(file, Cdr(term), in);	   	   /*                        */
      return;					   /*                        */
    case L_IF:				   	   /*                        */
      fputs("if (", file);			   /*                        */
      prn_args(file, Cdar(term), "", in + 1);	   /*                        */
      fputs(") ", file);			   /*                        */
      prn_term(file, Cadr(term), in);	   	   /*                        */
      if (Cddr(term))				   /*                        */
      { fputs(" else ", file);			   /*                        */
	prn_term(file, Cddr(term), in);	   	   /*                        */
      }						   /*                        */
      return;					   /*                        */
    case L_WITH:			   	   /*                        */
      fputs("with (", file);			   /*                        */
      prn_args(file, Car(term), ",", in + 1);	   /*                        */
      fputs(") ", file);			   /*                        */
      prn_term(file, Cdr(term), in);	   	   /*                        */
      return;					   /*                        */
    case L_UMINUS:				   /*                        */
      fputs("-", file);			   	   /*                        */
      prn_term(file, Cadr(term), in);		   /*                        */
      return;					   /*                        */
    case L_QUOTE:    key = "'";		     break;/*                        */
    case L_FUNCTION: key = (char*)TString(term);break;/*                     */
    case L_MINUS:    key = " - ";	     break;/*                        */
    case L_PLUS:     key = " + ";	     break;/*                        */
    case L_TIMES:    key = " * ";	     break;/*                        */
    case L_DIV:      key = " / ";	     break;/*                        */
    case L_MOD:      key = " mod ";          break;/*                        */
    case L_SET:      key = " = ";	     break;/*                        */
    case L_LIKE:     key = " like ";         break;/*                        */
    case L_ILIKE:    key = " ilike ";        break;/*                        */
    case L_EQ:       key = " == ";           break;/*                        */
    case L_NE:       key = " != ";           break;/*                        */
    case L_GT:       key = " > ";	     break;/*                        */
    case L_GE:       key = " >= ";           break;/*                        */
    case L_LT:       key = " < ";	     break;/*                        */
    case L_LE:       key = " <= ";           break;/*                        */
    case L_NOT:      key = "not";            break;/*                        */
    case L_AND:      key = " and ";          break;/*                        */
    case L_OR:       key = " or ";           break;/*                        */
    default:					   /*                        */
      fprintf(file, "?0x%x?", TType(term));	   /*                        */
      return;					   /*                        */
  }						   /*                        */
 						   /*                        */
  if (L_IS_BINARY(TType(term)) )		   /*                        */
  { term = Cdr(term);				   /*                        */
    if (term)					   /*                        */
    { fputc('(', file);				   /*                        */
      prn_term(file, Car(term), in);		   /*                        */
      fputs(key, file);		   		   /*                        */
      prn_term(file, Cadr(term), in);		   /*                        */
    } else {					   /*                        */
      return;					   /*                        */
    }						   /*                        */
  } else {					   /*                        */
    fputs(key, file);		   		   /*                        */
    fputc('(', file);				   /*                        */
    if (term) prn_args(file, Cdr(term), ", ", in); /*                        */
  }						   /*                        */
  fputc(')', file);				   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	print_term()
** Type:	void
** Purpose:	Produce a printed representation of a term and write it to
**		the output stream.
** Arguments:
**	file	the output stream
**	t	the term to print
** Returns:	nothing
**___________________________________________________			     */
void print_term(file, term)			   /*                        */
  register FILE * file;				   /*                        */
  register Term term;				   /*                        */
{ prn_term(file, term, 0);			   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	list_length()
** Type:	int
** Purpose:	Determine the length of a list, i.e. a cons sequence.
**		
** Arguments:
**	list	the list
** Returns:	the length
**___________________________________________________			     */
int list_length(list)				   /*                        */
  register Term list;				   /*                        */
{ int i = 0;					   /*                        */
  while (list && TermIsList(list))		   /*                        */
  { i++;					   /*                        */
    list = Cdr(list);				   /*                        */
  }						   /*                        */
  return i;					   /*                        */
}						   /*------------------------*/

/*-----------------------------------------------------------------------------
** Function:	symdef()
** Type:	SymDef
** Purpose:	Allocate a new symdef.
**		
** Arguments:
**	name	the name of the symdef
**	op	the op code
**	flags	the flags
**	get	the getter function
**	set	the setter function
** Returns:	
**___________________________________________________			     */
SymDef symdef(name, op, flags, get, set)	   /*                        */
  String name;					   /*                        */
  short int op;					   /*                        */
  short int flags;				   /*                        */
  Term (*get)_ARG((Binding, Term));		   /*                        */
  Term (*set)_ARG((Binding, Term));		   /*                        */
{						   /*                        */
  SymDef sym    = (SymDef) malloc(sizeof(SSymDef));/*                        */
  if (sym == SymDefNULL) OUT_OF_MEMORY("symdef");  /*                        */
  SymName(sym)  = name;				   /*                        */
  SymOp(sym)    = op;				   /*                        */
  SymFlags(sym) = flags;			   /*                        */
  SymHash(sym)  = hash(name);			   /*                        */
  SymTerm(sym)  = NIL;				   /*                        */
  SymValue(sym) = NIL;				   /*                        */
  SymGet(sym)   = get;			   	   /*                        */
  SymSet(sym)   = set;			   	   /*                        */
  return sym;					   /*                        */
}						   /*------------------------*/

/*---------------------------------------------------------------------------*/
