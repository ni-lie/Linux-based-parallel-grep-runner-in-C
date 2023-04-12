# Linux-based parallel grep runner in C

A multithreaded parallel grep runner program written in C that divides the workload of searching through all file contents in a directory
tree among parallel threads  

<h4> Sample Run </h4>

<b> String to be searched for: cs140 </b>
<b> Consider the list of directories and files in the image below </b>  
<ul>
    <li> walkthrough</li>
    <li> dir1</li>
    <li> dir2 </li>
    <li> dir3 </li>
    <li> dir4 </li>
    <li> dir5 </li>
    <li> 5 absent text files with contents that do not contain the string 'cs140' </li>
    <li> 1 present text file with content that do contain the string 'cs140' </li>
</ul> 


<p align="center">
    <img src="screenshots/directory.jpeg" alt="directory" />
</p> 



