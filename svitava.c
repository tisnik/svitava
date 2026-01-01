/*

   Svitava: fractal renderer

   (C) Copyright 2024, 2025, 2026  Pavel Tisnovsky

   All rights reserved. This program and the accompanying materials
   are made available under the terms of the Eclipse Public License v1.0
   which accompanies this distribution, and is available at
   http://www.eclipse.org/legal/epl-v10.html

   Contributors:
       Pavel Tisnovsky

*/

/*
build as shared library:
    gcc -shared -Wl,-soname,svitava -o svitava.so -fPIC svitava.c

build as executable:
    gcc -lm -o svitava svitava.c
*/

