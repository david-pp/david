" Use Vim settings, rather than Vi settings (much better!).
" This must be first, because it changes other options as a side effect.
set nocompatible

" allow backspacing over everything in insert mode
set backspace=indent,eol,start

syntax enable

let mapleader=","       " leader is comma

"
" UI Config
"
set number   " show line numbers
set showcmd  " show command in bottom bar
set cursorline          " highlight current line
"set cursorcolumn        " highlight current line


set incsearch " search as characters are entered
set hlsearch " highlight matches

" turn off search highlight
nnoremap <leader><space> :nohlsearch<CR>

"
" Spaces & Tabs
"
set tabstop=4  " number of visual spaces per TAB

set softtabstop=4  " number of spaces in tab when editing
set expandtab      " tabs are spaces

"
" Indent
"
filetype indent on      " load filetype-specific indent files
set shiftwidth=4


