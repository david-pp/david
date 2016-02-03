""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"
" David's vimrc
"
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" Use Vim settings, rather than Vi settings (much better!).
" This must be first, because it changes other options as a side effect.
set nocompatible

"
" Plugin Manager: vundle
"
filetype off " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

"
" Plugin: 代码自动补全
"
Plugin 'Valloric/YouCompleteMe'

"
" Plugin: 代码自动补齐配置生成器
"
Plugin 'rdnetto/YCM-Generator'

"
" Plugin: 配色方案
"
Plugin 'tomasr/molokai'

"
" Plugin: Fast file navigation for VIM
"
Plugin 'wincent/command-t'

"
" Plugin: A tree explorer plugin for vim
"
Plugin 'scrooloose/nerdtree'


" The following are examples of different formats supported.

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList          - list configured plugins
" :PluginInstall(!)    - install (update) plugins
" :PluginSearch(!) foo - search (or refresh cache first) for foo
" :PluginClean(!)      - confirm (or auto-approve) removal of unused plugins
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line

""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

" allow backspacing over everything in insert mode
set backspace=indent,eol,start

" 自动语法高亮
syntax on
" 配色方案
colorscheme molokai

" 设置行号
set number

" 突出显示光标所在行
set cursorline

" 状态栏
set ruler

" 设置TAB长度
set tabstop=4

" 设置自动换行时移动的跨度
set shiftwidth=4

" 搜索时忽略大小写，但在有一个或以上大写字母时仍保持对大小写敏感
set ignorecase smartcase

