kdtree
======

This project exists to develop a portable generic KDTree implementation. Though it needs to be as optimised as possible, since I plan to code an extensive photonmap on it ;)
For optimisation reasons, the code will be C++, and for genericity purpose, templates will be used.

For those who are wondering how a kdtree will work, here is a nice page about it, with an applet demo : http://www.rolemaker.dk/nonRoleMaker/uni/algogem/kdtree.htm

Here are projects that kdtree implementation was originally created for: http://asmodehn.home.dyndns.org/Photons
http://negre.guillaume.free.fr/html/CPP_renduRealiste.html

The CVS Repository is quite stable by now, since I use autotools to build the testing program. My testing system is NetBSD 2.0 (gcc 3.3.3). Because of portability questions concerning templates, I'll need some people who are interesting in testing it on other systems, and with other builder than gcc.

When this project will be mature for portability, optimized and widely used without any problems, multithreading support will be added (pthread).

Any help, comments or questions are welcome. 
