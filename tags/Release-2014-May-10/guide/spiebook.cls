%% spiejour.cls - Version 1.00 - 27 September 2006
%% LaTeX2e class file to format manuscript for SPIE e-journals
%  Requires following LaTeX packages:
%  times.sty  % default font for this class
%  float.sty
%  ifthen.sty
%  cite.sty  % version 4.01 or later
%  color.sty  % colored fonts
% History
%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Adapted from spie.cls by Kenneth Hanson  27 September 2006%
%%%%%%%%%%%%%%%%%%%%%%%%
% Usage:
% \documentclass[]{spie}  %% US letter paper size - req. by SPIE
% \documentclass[a4paper]{spie}  %% use this for A4 paper
%
% This style file does not automatically implement all the formatting
%  required for an SPIE manuscript.  The user must make a few 
%  manual adjustments For proper formatting, such as:
% 1. Title, subsection, and subsubsection should be in lower case 
%    except for the first letter and proper nouns or acronyms
% 2. Format authors and their affiliations as follows
%    \author{author1\supscr{a}, author2\supscr{b}, and author3\supscr{c} )
%    \affiliation{\supscr{a}affiliation1\\
%                 \supscr{b}affiliation2\\
%                 \supscr{c}affiliation3
%                }
%
%%%  Note: \supscr{} is defined in this class
% ------------------------------------------------------------
%%  Based on standard LaTeX article class --
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{spiebook}[2007/04/11  v0.01 SPIE book class]

\PassOptionsToClass{11pt}{book}
\DeclareOption{a4paper}{%
   \AtEndOfClass{%
%       \oddsidemargin 1.31cm    % for side margins of 3.85 cm
%       \evensidemargin 1.31cm   % for side margins of 3.85 cm
       \typeout{a4paper used}
       }
   }



\ProcessOptions*      % set margins for a4paper if specified
\LoadClass{book}  % build on standard article class




%% type out specified font size
\if0\@ptsize\typeout{ten-point font}\fi
\if1\@ptsize\typeout{eleven-point font}\fi
\if2\@ptsize\typeout{twelve-point font}\fi

\IfFileExists{times.sty}{\RequirePackage[T1]{fontenc}\RequirePackage{times}}{%
  \IfFileExists{lmodern}{\RequirePackage[T1]{fontenc}\RequirePackage{lmodern}}{%
    \renewcommand\rmdefault{utm}%
    \renewcommand\sfdefault{uhv}%
    \renewcommand\ttdefault{ucr}%
}%
}

\RequirePackage{float}
\RequirePackage{ifthen}
\RequirePackage{cite}[2003/11/04] % need vers. > 4.01 
\RequirePackage{color}
%% page format (see "Sample manuscript showing specifications and style")
%% following based on default top and left offset of 1 inch = 25.4mm
%% the next two side margins are for US letter paper 
%% and are overridden by the a4paper option
\oddsidemargin 0.75in           % for side margin of 1.75 in
\evensidemargin 0.75in          % for side margin of 1.75 in
%
\textheight 8in                % approx 20.32 cm
\textwidth 5in                 % approx 12.7 cm
\topsep 11\p@
\headheight 12\p@ \headsep 3pc      % avoid extra space for header
\advance\headsep by -\topsep
\topmargin 0.7in                % for top margin of 1.7in
\advance\topmargin by -\headheight
\advance\topmargin by -\headsep
\setlength{\parskip}{\z@} % spacing between paragraph
%%\parskip 1ex plus 1ex minus 0.3ex % spacing between paragraphs
\date{}                           % avoid date

%%  space for floats - figures and tables
\setlength{\abovecaptionskip}{0ex}
\setlength{\floatsep}{3ex plus 0.3ex minus 0.3ex} 
\setlength{\textfloatsep}{ 3ex plus 3ex minus 1.5ex}
\renewcommand{\textfraction}{0.10} 
\renewcommand{\floatpagefraction}{0.60} 
\renewcommand{\topfraction}{0.90} 
\renewcommand{\bottomfraction}{0.90}
\setcounter{totalnumber}{3} 
\setcounter{topnumber}{2} 
\setcounter{bottomnumber}{2} 
\setlength{\doublerulesep}{\arrayrulewidth}


%%%% useful definitions %%%%
\def\skiplinehalf{\medskip\\}
\def\skipline{\\}
\def\exspace{\vspace{1ex}}
% superscript, to cross reference author and affiliations
\def\supscr#1{\raisebox{0.8ex}{\small #1}\hspace{0.05em}}  
% superscript in italics, 
\def\supit#1{\raisebox{0.8ex}{\small\it #1}\hspace{0.05em}}  

\newcommand{\LB}[1][3]{\linebreak[#1]} % allows linebreak with default of level 3; \LB[2] uses level 2
\newcommand{\linkable}[1]{\textcolor{blue}{\underline{#1}}}
%%%%%%%%%%%%%%%%%%%%
%%%%  define \ample font size %%%%
%% 10% larger than \normalsize for 10 pt, 
%% but smaller than \large
\def\ample{\large}   % insurance

\def\setamplefont{
\if0\@ptsize    % 10pt text; use 11pt bold fonts
  \def\ample{\@setsize\large{12pt}\xipt\@xipt}
\fi

\if1\@ptsize    % 10pt text; use 11pt bold fonts
  \def\ample{\@setsize\large{12pt}\xiipt\@xiipt}
\fi

\if2\@ptsize    % 12pt text; use 13.2pt bold fonts
  \renewcommand{\ample}{\fontsize{13.2pt}{15.8pt}\selectfont\bfseries} 
  \ifcmr \font\ample = cmbx17 at 13.2truept  \fi
\fi  }


%%%% define title aspects %%%%
\def\titlefont{\normalfont\sffamily\fontsize{24}{30}\bfseries}  %% insurance
\def\chapternamefont{\huge\bfseries}
\def\title#1{\thispagestyle{empty}\gdef\@title{\titlefont{\chapternamefont
      \chaptername\ \thechapter}\\ #1}} 
\def\runningchapter#1{\markboth{#1}{}}
\def\runningtitle#1{\markright{#1}}


\def\settitlefont{
%%% identify font used; cmr = Computer Modern Roman, ptm = Times Roman
\typeout{\rmdefault -font_in_spiecls}
\def\cmr{cmr}  % variables for font tests
\newif\ifcmr
\def\ptm{ptm}
\newif\ifptm
\ifthenelse{\equal{\rmdefault}{\cmr}}{\cmrtrue}{\cmrfalse}
\ifthenelse{\equal{\rmdefault}{\ptm}}{\ptmtrue}{\ptmfalse}
%%% scale title according to default font and size
\if0\@ptsize    % 10pt text; use 16pt bold fonts
  \renewcommand{\titlefont}{\fontsize{16pt}{19.2pt}\selectfont\bfseries}
  \ifcmr  \font\titlefont = cmbx12 at 16truept  \fi
\fi
% for 11pt text, title font should be 17pt = \LARGE for 11pt. No change needed
\if2\@ptsize    % 12pt text; use 19.2pt bold fonts
  \renewcommand{\titlefont}{\fontsize{19.2pt}{23pt}\selectfont\bfseries}
  \ifcmr  \font\titlefont = cmbx12 at 19.2truept  \fi
\fi  }

\def\authorinfo#1{\gdef\@authorinfo{#1}}
\authorinfo{}     %% default is empty
\let\@affiliation\@empty
\def\affiliation#1{\gdef\@affiliation{#1}}

%% Chapter
%\let\chapter\title

\renewcommand\chapter{\if@openright\cleardoublepage\else\clearpage\fi
                    \thispagestyle{empty}%
                    \global\@topnum\z@
                    \@afterindentfalse
                    \secdef\@chapter\@schapter}
\def\@chapter[#1]#2{\ifnum \c@secnumdepth >\m@ne
                       \if@mainmatter
                         \refstepcounter{chapter}%
                         \typeout{\@chapapp\space\thechapter.}%
                         \addcontentsline{toc}{chapter}%
                                   {\protect\numberline{\thechapter}#1}%
                       \else
                         \addcontentsline{toc}{chapter}{#1}%
                       \fi
                    \else
                      \addcontentsline{toc}{chapter}{#1}%
                    \fi
                    \chaptermark{#1}%
                    \addtocontents{lof}{\protect\addvspace{10\p@}}%
                    \addtocontents{lot}{\protect\addvspace{10\p@}}%
                    \if@twocolumn
                      \@topnewpage[\@makechapterhead{#2}]%
                    \else
                      \@makechapterhead{#2}%
                      \@afterheading
                    \fi}
\def\@makechapterhead#1{%
  \vspace*{50\p@}%
  {\parindent \z@ \raggedright \normalfont
    \ifnum \c@secnumdepth >\m@ne
      \if@mainmatter
        \fontsize{20}{24}\sffamily\bfseries \@chapapp\space \thechapter
        \par\nobreak
        \vskip 4\p@
      \fi
    \fi
    \interlinepenalty\@M
    \fontsize{24}{30} \bfseries #1\par\nobreak
    \vskip 20\p@
  }}
\def\@schapter#1{\if@twocolumn
                   \@topnewpage[\@makeschapterhead{#1}]%
                 \else
                   \@makeschapterhead{#1}%
                   \@afterheading
                 \fi}
\def\@makeschapterhead#1{%
  \vspace*{50\p@}%
  {\parindent \z@ \raggedright
    \normalfont
    \interlinepenalty\@M
    \fontsize{24}{30}\sffamily \bfseries  #1\par\nobreak
    \vskip 20\p@
  }}


\def\runninghead@font{\sffamily\fontsize{9}{11}\itshape}
\def\runninghead@rule{\hbox to0pt{\vbox{\hrule height-2.5\p@ depth3pt width\textwidth}\hss}}
\def\ps@headings{%
      \let\@oddfoot\@empty\let\@evenfoot\@empty
      \def\@evenhead{\runninghead@rule\@gobble{\thepage}\hfil\runninghead@font\leftmark}%
      \def\@oddhead{\runninghead@rule{\runninghead@font\rightmark}\hfil\@gobble{\thepage}}%
      \let\@mkboth\markboth%
}


%%%%  define footnote attributes %%%%
\renewcommand{\footnotesize}{\small}  % enlarge footnote font to small
\renewcommand{\thefootnote}{\fnsymbol{footnote}}
\ifx\@authorinfo\empty \else\footnotetext[0]{\@authorinfo}\fi      
%% use zero to avoid footnote mark
%\renewcommand{\thefootnote}{\fnsymbol{footnote}}  
%% use footnote symbols, not numbers

\let\sv@footnote=\footnote
\def\footnote{\ifnum\c@footnote>8\setcounter{footnote}{0}\fi\sv@footnote}




%%%% section aspects %%%%
% all headings bold
% all section headings, ample size, flush left
\def\sectfont{\raggedright\sffamily\bfseries\large}
\def\subsectfont{\raggedright\sffamily\bfseries\normalsize}
\def\subsubsectfont{\raggedright\sffamily\bfseries\small}

\def\section{\@startsection{section}{1}{\z@}
   {-2.5ex plus -1ex minus -0.5ex}{1.2ex plus 0.5ex minus 0ex}{\sectfont}} 
\def\subsection{\@startsection{subsection}{2}{\z@}
   {-2.5ex plus -1ex minus -0.5ex}{1.1ex plus 0.1ex minus 0.1ex}{\subsectfont}}
\def\subsubsection{\@startsection{subsubsection}{3}{\z@}
   {-2.5ex plus -1ex minus -0.5ex}{1.1ex plus 0.1ex}{\subsubsectfont}}
\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                                    {3.25ex \@plus1ex \@minus.2ex}%
                                    {-1em}%
                                    {\normalfont\small\sffamily\itshape}}

\setcounter{secnumdepth}{3}
\gdef\sectionmark#1{}
%\def\chaptermark#1{\markleft{#1}}
%
\def\@seccntformat#1{\csname the#1\endcsname\enskip}

%%%% references %%%%
\def\citeleft{}
\def\citeright{}
\def\cite@fmt#1{\ensuremath{{}^{\mbox{\scriptsize #1}}}}


%\RequirePackage{cite}
% Hacking cite.sty
\def\@cite#1#2{\leavevmode %\cite@adjust
  \cite@fmt{\citeleft{#1\if@tempswa \citemid #2\fi
  \spacefactor\@m % punctuation in note doesn't affect outside
  }\citeright}}
\def\inlinecite#1{\bgroup\let\cite@fmt\relax\cite{#1}\egroup}




% Give the references section a section number
\def\thebibliography#1{\section*{References}%
  \addcontentsline{toc}{section}{References}{}%
 \list
 {\arabic{enumi}.~}   % kmh - was  {\arabic{enumi}.}
 {\settowidth\labelwidth{#1.~}\leftmargin\labelwidth
 \advance\leftmargin\labelsep \usecounter{enumi}}
 \def\newblock{\hskip .11em plus .33em minus .07em}
  %% reduce vspace between items
  \parskip 0.2ex plus 0.001ex minus 0ex   
% \sloppy\clubpenalty4000\widowpenalty4000
 \sfcode`\.=1000\relax}
\let\endthebibliography=\endlist


%%%% Add theorem, lemma, and definition environments %%%%
% kmh - noindent
\def\@begintheorem#1#2{\addvspace{2ex}%
   \par\noindent\bgroup{#1\ #2. }\ignorespaces}
\def\@opargbegintheorem#1#2#3{\addvspace{2ex}%
   \par\noindent\bgroup{ #1\ #2\ (#3). }\ignorespaces}
\def\@endtheorem{\egroup}
\def\proof{\par{\it Proof}. \ignorespaces}
\def\endproof{{\ \vbox{\hrule\hbox{%
   \vrule height1.3ex\hskip0.8ex\vrule}\hrule
  }}\par}
\newtheorem{theorem}{Theorem}
\newtheorem{lemma}{Lemma}
\newtheorem{definition}{Definition}

%%%% Figure and table captions %%%
\def\figurenamefont{\sffamily\fontsize{9}{11}\bfseries}
\def\figurefont{\sffamily\fontsize{9}{11}\selectfont}
\renewcommand\figurename{Figure}
\long\def\@makecaption#1#2{%     % from article.cls
  \vskip\abovecaptionskip
  \sbox\@tempboxa{{{\figurenamefont #1}  \figurefont#2}}%
  \ifdim \wd\@tempboxa >\hsize   % unbold with period
    {{\figurenamefont #1} \figurefont#2 \par}
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}


%%%% appendix aspects %%%% 
% use \appendix to start an appendix
% use \section{} for each appendix section
\def\appendix{\def\append{1}
  \par
  \setcounter{section}{0}
  \setcounter{subsection}{0}
  \setcounter{subsubsection}{0}
  \def\thesection{\Alph{section}}  
  \def\thesubsection{\Alph{section}.\arabic{subsection}}
\def\thesubsubsection{
\Alph{section}.\arabic{subsection}.\arabic{subsubsection}}
}

%% Table of Contents
\renewcommand\@pnumwidth{1.55em}
\renewcommand\@tocrmarg{0.00em}
\renewcommand\@dotsep{10000}
%
\def\pagenum@shape{\normalsize\sffamily}
\def\part@shape{}
\def\chapter@shape{}
\def\section@shape{\normalsize\sffamily}
\def\subsection@shape{\normalsize\sffamily}
\def\subsubsection@shape{\normalsize\sffamily}
%
\def\l@chapter@hook{}
\def\l@section@hook{\section@shape}
\def\l@subsection@hook{\normalsize\subsection@shape\@tempdimb\cw@chapter\advance\@tempdimb by\cw@section}
\def\l@subsubsection@hook{\normalsize\subsubsection@shape\@tempdimb\cw@chapter\advance\@tempdimb by\cw@section}

\setcounter{tocdepth}{2}
\def\contentsline#1#2#3{\bgroup\let\num@dot@sep\enspace
  \csname l@#1@hook\endcsname\csname l@#1\endcsname{#2}{\@gobble{#3}}\egroup}
%
\def\max@cw@dim#1#2{%
  \bgroup
    \let\num@dot@sep\enspace
%    \def\num@dot@sep{\hskip0pt}
    \csname l@#1@hook\endcsname
    \setbox\@tempboxa\hbox{#2\num@dot@sep}%
    \@ifundefined{cw@#1}{\expandafter\newdimen\csname cw@#1\endcsname}{}%
    \global\csname cw@#1\endcsname\wd\@tempboxa
  \egroup
  }
\def\maxcchap#1{\max@cw@dim{chapter}{#1}}
\def\maxcsec#1{\max@cw@dim{section}{#1}}
\def\maxcssec#1{\max@cw@dim{subsection}{#1}}
\AtBeginDocument{%
  \maxcchap{}%
  \maxcsec{1.1}%
  \maxcssec{1.1.1}%
  }


\renewcommand*\l@part[2]{}
\renewcommand*\l@chapter[2]{}

\renewcommand*\l@section{\@dottedtocline{1}{\z@}{\cw@section}}
\renewcommand*\l@subsection{\@dottedtocline{2}{\cw@section}{\cw@subsection}}
\renewcommand*\l@subsubsection{\@dottedtocline{3}{7.0em}{4.1em}}
\renewcommand*\l@paragraph{\@dottedtocline{4}{10em}{5em}}
\renewcommand*\l@subparagraph{\@dottedtocline{5}{12em}{6em}}

\renewcommand\tableofcontents{%
  \medskip
    \if@twocolumn
      \@restonecoltrue\onecolumn
    \else
      \@restonecolfalse
    \fi
    \@starttoc{toc}%
    \if@restonecol\twocolumn\fi
\medskip
    }


%% Authors
\newenvironment{aug}{%
\def\author##1{\noindent\bgroup\sffamily\bfseries\large##1\egroup\par}%
\def\affilation##1{\noindent\bgroup\sffamily##1\egroup\par}%
}{\addvspace{\bigskipamount}}
\pagestyle{headings}
\endinput
%% end of spiebook.cls