/*
Copyright (C) 2023, Sorbonne Universite, LIP6
This file is part of the VerifMSI++ project, under the GPL v3.0 license
See https://www.gnu.org/licenses/gpl-3.0.en.html for license information
SPDX-License-Identifier: GPL-3.0-only
Author(s): Quentin L. Meunier
*/


#ifndef _config_hpp_
#define _config_hpp_

#define DELETE_NODES 1
#define KEEP_NODES   2

#define MEMORY_STRATEGY         KEEP_NODES
#define PROPAGATE_CST_ON_BUILD  true
#define BIT_EXP_ENABLE          true
#define EXTENDED_SIMPLIFY       true
#define BIT_SIMPLIFY_PLUS       true


#define REM_SINGLE_INPUT_PROBES true
#define REM_REDUNDANT_PROBES    true
#define BARTHE_OPT              false

#endif

