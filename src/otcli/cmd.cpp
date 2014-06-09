/* See other files here for the LICENCE that applies here. */
/* See header file .hpp for info */

#include "cmd.hpp"

#include "lib_common2.hpp"

namespace nOT {
namespace nNewcli {

INJECT_OT_COMMON_USING_NAMESPACE_COMMON_2; // <=== namespaces

using namespace nUse;

// ========================================================================================================================

class cCmdParser_pimpl {
	friend class cCmdParser;

	typedef map< cCmdName , shared_ptr<cCmdFormat> >::value_type tTreePair; // type of element (pair) in tree-map. TODO: will be not needed in C+11 map emplace

	private:
		map< cCmdName , shared_ptr<cCmdFormat> > mTree;
};

cCmdParser::cCmdParser() 
: mI( new cCmdParser_pimpl )
{ }

cCmdExecutable::tExitCode Execute1( shared_ptr<cCmdData> , nUse::cUseOT ) {
	_mark("***Execute1***");
	return cCmdExecutable::sSuccess;
}

void cCmdParser::AddFormat( const cCmdName &name, shared_ptr<cCmdFormat> format ) {
	mI->mTree.insert( cCmdParser_pimpl::tTreePair ( name , format ) );
	_info("Add format for command name (" << (string)name << "), now size=" << mI->mTree.size() << " new format is: ");
	format->Debug();
}

void cCmdParser::AddFormat(
			const string &name, 
			const vector<cParamInfo> &var,
			const vector<cParamInfo> &varExt,
			const map<string, cParamInfo> &opt,
			const cCmdExecutable::tFunc &exec)
{
	auto format = std::make_shared< cCmdFormat >( cCmdExecutable(exec), var, varExt, opt );
	AddFormat(name, format);
}

void cCmdParser::Init() {
	_mark("Init tree");
	/*
	typedef function< bool ( cUseOT &, cCmdData &, int, const string &  ) > tFuncValid;
	typedef function< vector<string> ( cUseOT &, cCmdData &, int, const string &  ) > tFuncHint;
	cParamInfo(tFuncValid valid, tFuncHint hint);
	*/

	cParamInfo pNym(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym validation");
				return use.NymCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Nym hinting");
			return use.NymGetAllNames();
		}
	);
	cParamInfo pNymFrom = pNym;
	cParamInfo pNymTo = pNym; // TODO suggest not the same nym as was used already before
	cParamInfo pNymAny = pNym;

	cParamInfo pNymNewName(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Nym name validation");
				return true; // Takes all input TODO check if Nym with tis name exists
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Nym name hinting");
			return vector<string> {}; // No hinting for new Nym name
		}
	);

	cParamInfo pAccount(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Account validation");
				return use.AccountCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Account hinting");
			return use.AccountGetAllNames();
		}
	);

	cParamInfo pAccountNewName(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Account name validation");
				return true; // Takes all input TODO check if Account with tis name exists
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Account name hinting");
			return vector<string> {}; // No hinting for new Nym name
		}
	);


	cParamInfo pAsset(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Asset validation");
				return use.AssetCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Asset hinting");
			return use.AssetGetAllNames();
		}
	);

	cParamInfo pServer(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			_dbg3("Server validation");
				return use.ServerCheckIfExists(data.Var(curr_word_ix + 1));
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			_dbg3("Server hinting");
			return use.ServerGetAllNames();
		}
	);

	cParamInfo pOnceInt(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			// TODO check if is any integer
			// TODO check if not present in data
			return true;
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "-1", "0", "1", "2", "100" };
		}
	);

	cParamInfo pSubject(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			return true;
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "hello","hi","test","subject" };
		}
	);

	cParamInfo pBool(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			return true; // option's value should be null
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; // this should be empty option, let's continue
		}
	);
	// sendmoney alice gold 1000
	// sendmsgto alice hi    --addmoney 1000 --addmoney 2000  
	//           arg=1 arg=2           arg=3           arg=4
	// TODO 

	cParamInfo pMsgInIndex(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			const int nr = curr_word_ix+1;
			return use.MsgInCheckIndex(data.Var(nr-1), std::stoi( data.Var(nr)) ); //TODO check if integer
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; //TODO hinting function for msg index
		}
	);

	cParamInfo pMsgOutIndex(
		[] (cUseOT & use, cCmdData & data, size_t curr_word_ix ) -> bool {
			const int nr = curr_word_ix+1;
			return use.MsgOutCheckIndex(data.Var(nr-1), std::stoi( data.Var(nr)) ); //TODO check if integer
		} ,
		[] ( cUseOT & use, cCmdData & data, size_t curr_word_ix  ) -> vector<string> {
			return vector<string> { "" }; //TODO hinting function for msg index
		}
	);

	// ===========================================================================

	using namespace nOper; // vector + is available inside lambdas
	using std::stoi;

	// types used in lambda header:
	typedef shared_ptr<cCmdData> tData;
	typedef nUse::cUseOT & tUse;
	typedef cCmdExecutable::tExitCode tExit;

	auto & pFrom = pNymFrom;
	auto & pTo = pNymTo;
	auto & pSubj = pSubject;
	auto & pMsg = pSubject; // TODO
	auto & pInt = pOnceInt;

	#define LAMBDA [] (tData d, tUse U) -> tExit

	AddFormat("msg sendfrom", {pFrom, pTo}, {pMsg, pSubj}, { {"--dryrun",pBool} , {"--cc",pNym} , {"--bcc",pNym} , {"--prio",pInt} },
		LAMBDA { auto &D=*d; return U.MsgSend(D.V(1), D.V(2) + D.o("--cc") , D.v(3), D.v(4,"nosubject"), stoi(D.o1("--prio","0")), D.has("--dryrun")); } );

//	Prepare format for all msg commands:
//	 	 "ot msg ls"
//		,"ot msg ls alice"
//		,"ot msg sendfrom alice bob hello --cc eve --cc mark --bcc john --prio 4"
//		,"ot msg sendto bob hello --cc eve --cc mark --bcc john --prio 4"
//		,"ot msg rm alice 0"
//		,"ot msg-out rm alice 0"


	{ // FORMAT: msg ls
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use ) -> cCmdExecutable::tExitCode {
				_mark("Try vardef");
				string nymName = data->VarDef(1,"");
				_mark("... s="<<nymName);
				if ( nymName.empty() ) {
					_dbg3("Execute MsgGetAll()");
					use.MsgGetAll();
				}else {
					_dbg3("Execute MsgGetforNym(" + nymName + ")");
					use.MsgGetForNym(nymName);
				}
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
			varExt.push_back( pNymAny );
		cCmdFormat::tOption opt;
		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("msg ls") , format );
	}

	{ // FORMAT: msg sendto
		// ot msg sendto bob subj
		// ot msg sendto NYM_TO SUBJ
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string to = data->Var(1);
				string subj = data->VarDef(2,"");
				string prio = data->Opt1If("prio", "0");

				_note(" to " << to << " subj=" << subj << " prio="<<prio);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
					for(auto cc : data->OptIf("cc")) _note("--cc to " << cc);
					use.MsgSend( use.NymGetDefault(), data->Var(1), data->VarDef(2,"no_subject") );
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNymTo );
		cCmdFormat::tVar varExt;
			varExt.push_back( pSubject );
		cCmdFormat::tOption opt;
			opt.insert(std::make_pair("--dryrun" , pNymAny)); // TODO should be global option
			opt.insert(std::make_pair("--cc" , pNymAny));
			opt.insert(std::make_pair("--bcc" , pNymAny));
			opt.insert(std::make_pair("--prio" , pOnceInt));

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("msg sendto") , format );
	}

	{ // FORMAT: msg rm
		// ot msg rm alice 	0
		// ot msg rm NYM 		INDEX
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);
				string index = data->Var(2);

				_note(" rm message from " << nym << " mailbox, with index=" << index );
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.MsgInRemoveByIndex(nym, std::stoi(index)); //TODO check if integer
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
			var.push_back( pOnceInt );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;
			opt.insert(std::make_pair("--all" , pOnceInt)); // FIXME proper handle option without parameter!

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("msg rm") , format );
	}

	{ // FORMAT: msg-out rm
		// ot msg-out rm alice 	0
		// ot msg-out rm NYM 		INDEX
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);
				string index = data->Var(2);

				_note(" rm message from " << nym << " mailbox, with index=" << index );
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.MsgOutRemoveByIndex(nym, std::stoi(index)); //TODO check if integer
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
			var.push_back( pOnceInt );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;
			opt.insert(std::make_pair("--all" , pOnceInt)); // FIXME proper handle option without parameter!

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("msg rm-out") , format );
	}

	//	,"ot nym check alice"
	//	,"ot nym info alice"
	//	,"ot nym register alice"
	//	,"ot nym rm alice"
	//	,"ot nym new alice"
	//	,"ot nym set-default alice"
	//	,"ot nym refresh"
	//	,"ot nym refresh alice"
	//	,"ot nym ls"

	{ // FORMAT: nym check
		// ot nym check alice
		// ot nym check NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);

				_note("check nym " << nym );
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.NymCheck(nym);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym check") , format );
	}

	{ // FORMAT: nym info
		// ot nym info alice
		// ot nym info NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);

				_note("info for nym " << nym );
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.NymGetInfo(nym);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym info") , format );
	}

	{ // FORMAT: nym register
		// ot nym register alice
		// ot nym register NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);
				string server = data->VarDef( 2, use.ServerGetName( use.ServerGetDefault() ) ); //TODO do it simpler
				_note("registering nym=" << nym << " on server=" << server);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.NymRegister(nym, server);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
		cCmdFormat::tVar varExt;
			varExt.push_back( pServer );
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym register") , format );
	}

	{ // FORMAT: nym rm
		// ot nym rm alice
		// ot nym rm NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);

				_note("remove nym=" << nym );
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.NymRemove(nym);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym rm") , format );
	}

	{ // FORMAT: nym new
		// ot nym new alice
		// ot nym new NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);

				_note("new nym=" << nym );
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.NymCreate(nym);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNymNewName );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym new") , format );
	}

	{ // FORMAT: nym set-default
		// ot nym set-default alice
		// ot nym set-default NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->Var(1);
				_note("Set default nym=" << nym);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.NymSetDefault(nym);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym set-default") , format );
	}

	{ // FORMAT: nym refresh
		// ot nym refresh alice
		// ot nym refresh NYM
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->VarDef( 1, use.NymGetName( use.NymGetDefault() ) );
				_note("refresh nym=" << nym);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				if (nym.empty())
					use.NymRefreshAll();
				else
					use.NymRefresh(nym);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pNym );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym refresh") , format );
	}

	{ // FORMAT: nym ls
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use ) -> cCmdExecutable::tExitCode {
				_note("nym ls");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				nUtils::DisplayVector(cout, use.NymGetAllNames() );
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;
		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("nym ls") , format );
	}

	//	,"ot account new assetname accountname"
	//	,"ot account refresh"
	//	,"ot account refresh accountname"
	//	,"ot account set-default accountname"
	//	,"ot account rm accountname"
	//	,"ot account ls"
	//	,"ot account mv accountname newaccountname"

	{ // FORMAT: account new
		// ot account new assetname accountname
		// ot account new assetName accountName
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string asset = data->Var(1);
				string account = data->Var(2);
				_note("New nym=" << account);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.AccountCreate(asset, account);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pAccount );
			var.push_back( pAsset );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("account new") , format );
	}

	{ // FORMAT: account refresh
		// ot account refresh alice
		// ot account refresh account
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string account = data->VarDef( 1, use.AccountGetName( use.AccountGetDefault() ) );
				_note("refresh account=" << account);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				if (account.empty())
					use.AccountRefreshAll();
				else
					use.AccountRefresh(account);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pAccount );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("account refresh") , format );
	}

	{ // FORMAT: account set-default
		// ot account set-default accountname
		// ot account set-default account
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string account = data->Var(1);
				_note("Set default account=" << account);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.AccountSetDefault(account);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pAccount );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("account set-default") , format );
	}

	{ // FORMAT: account rm
		// ot account rm accountname
		// ot account rm account
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string account = data->Var(1);
				_note("Remove account=" << account);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.AccountRemove(account);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pAccount );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("account rm") , format );
	}

	{ // FORMAT: account ls
		// ot account ls accountname
		// ot account ls account
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				_note("ls account");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				nUtils::DisplayVector( cout, use.AccountGetAllNames() );
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("account ls") , format );
	}

	{ // FORMAT: account mv
		// ot account ls accountname
		// ot account ls account
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string accountName = data->Var(1);
				string accountNewName = data->Var(2);
				_note("mv account=" << accountName << "to new=" << accountNewName);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.AccountRename(accountName, accountNewName);
				return 0;
			}
		);
		cCmdFormat::tVar var;
		var.push_back( pAccount );
		var.push_back( pAccountNewName );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("account mv") , format );
	}

	//	,"ot asset new"
	//	,"ot asset issue"
	//	,"ot asset ls"

	{ // FORMAT: asset ls
		// ot asset ls
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				_note("ls asset");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				nUtils::DisplayVector( cout, use.AssetGetAllNames() );
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("asset ls") , format );
	}

	{ // FORMAT: asset issue
		// ot asset issue
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string server = data->VarDef( 1, use.ServerGetDefault() );
				string nym = data->VarDef( 2, use.NymGetDefault() );
				string contract; // TODO get multiline contract
				_note("Issue asset by nym=" << nym << " on server=" << server);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.AssetIssue(server, nym, contract);
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
			varExt.push_back( pServer );
			varExt.push_back( pNym );
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("asset issue") , format );
	}

	{ // FORMAT: asset new
		// ot asset new
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string nym = data->VarDef( 1, use.NymGetDefault() );
				string xmlContents; // TODO get multiline xml
				_note("new asset");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.AssetNew(nym, xmlContents);
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
			varExt.push_back( pNym );
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("asset new") , format );
	}

	//	,"ot server add"
	//	,"ot server ls"
	//	,"ot server new"
	//	,"ot server rm servername"
	//	,"ot server set-default servername"

	{ // FORMAT: server ls
		// ot server ls
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				_note("ls server");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				nUtils::DisplayVector( cout, use.ServerGetAllNames() );
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("server ls") , format );
	}

	{ // FORMAT: server add
		// ot server add
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string contract; //TODO get multiline contract
				_note("add server");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.ServerAdd(contract);
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("server add") , format );
	}

	{ // FORMAT: server new TODO implement functionality
		// ot server new
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string contract; //TODO get multiline contract
				_note("new server");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				//use.Server(contract);
				return 0;
			}
		);
		cCmdFormat::tVar var;
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("server new") , format );
	}

	{ // FORMAT: server rm
		// ot server rm
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string server = data->Var(1); //TODO get multiline contract
				_note("rm server");
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.ServerRemove(server);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pServer );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("server rm") , format );
	}

	{ // FORMAT: server set-default
		// ot server set-default servername
		// ot server set-default server
		cCmdExecutable exec(
			[] ( shared_ptr<cCmdData> data, nUse::cUseOT & use) -> cCmdExecutable::tExitCode {
				string server = data->Var(1);
				_note("Set default server=" << server);
				if (data->IsOpt("dryrun")) _note("Option dryrun is set");
				use.ServerSetDefault(server);
				return 0;
			}
		);
		cCmdFormat::tVar var;
			var.push_back( pServer );
		cCmdFormat::tVar varExt;
		cCmdFormat::tOption opt;

		auto format = std::make_shared< cCmdFormat >( exec , var, varExt, opt );
		AddFormat( cCmdName("server set-default") , format );
	}

	// TODO multiline support before implementing this:
	//	,"ot text encode texttoprocess"
	//	,"ot text decode text"
	//	,"ot text encrypt bob text"
	//	,"ot text decrypt text"



	//mI->tree.emplace( cCmdName("msg send") , msg_send_format );
	
//	mI->tree[ cCmdName("msg send") ] = msg_send_format;

	// msg sendfrom bob alice
	// msg sendfrom bob alice HelloThisIsATest // TODO, other call to OTUse, just pass the message
	// msg sendfrom bob alice "Hello This Is A Test" // TODO, need parser+editline support for quotes

/*	mI->tree[ cCmdName("msg send") ] = cCmdFormat( 
			vector<cParamInfo>{ ARG_STR, ARG_STR, ARG_STR }, map<string,cParamInfo>{{"subject",ARG_STR}}, map<string,cParamInfo>{{"cc",ARG_STR}} ,
	 		[]( nUse::cUseOT &useOt , cCmdData &data ) { 
				string msg=data.arg(3,""); if (0==msg.length()) msg=nUtils::GetMultiline(); 
				useOt->msgSend( data.arg(1), data.arg(2), msg ); }
			);

	
	typedef vector<cParamInfo> vpar;
	typedef map<string,cParamInfo> mopt;

	mI->tree[ cCmdName("msg send") ] = cCmdFormat( useOt::msgList, 
		vpar{ ARG_STR, ARG_STR, ARG_STR },  vpar{},  vopt{{"subject",ARG_STR}},  mopt{{"cc",ARG_STR}, {"bcc",ARG_STR}} );
*/
}


cCmdProcessing cCmdParser::StartProcessing(const vector<string> &words, shared_ptr<nUse::cUseOT> use ) {
	return cCmdProcessing( shared_from_this() , words , use );
}

cCmdProcessing cCmdParser::StartProcessing(const string &words, shared_ptr<nUse::cUseOT> use ) {
	_dbg3("Will split words: [" << words << "]");
	return cCmdProcessing( shared_from_this() , nUtils::SplitString(words) , use );
}

shared_ptr<cCmdFormat> cCmdParser::FindFormat( const cCmdName &name ) 
	throw(cErrParseName)
{
	auto it = mI->mTree.find( name );
	if (it == mI->mTree.end()) {
		throw cErrParseName("No such ot command="+(string)name);
	}
	return it->second;
}

// ========================================================================================================================

cCmdName::cCmdName(const string &name) : mName(name) { }

bool cCmdName::operator<(const cCmdName &other) const { return mName < other.mName; }

cCmdName::operator std::string() const { return mName; }

// ========================================================================================================================


cCmdProcessing::cCmdProcessing(shared_ptr<cCmdParser> parser, vector<string> commandLine, shared_ptr<nUse::cUseOT> &use )
	: mParser(parser), mCommandLine(commandLine), mUse(use)
{ 
	_dbg2("Creating processing of: " << DbgVector(commandLine) );
	_dbg2("Working on use=" << use->DbgName() );
}

void cCmdProcessing::Parse() {
	int _dbg_ignore=50;

	// mCommandLine = ot, msg, sendfrom, alice, bob, hello
	// mFormat.erase ? // remove old format, we parse something new [doublecheck]

	if (!mCommandLine.empty()) {
		if (mCommandLine.at(0) != "ot") {
			_warn("Command for processing is mallformed");
		}
		mCommandLine.erase( mCommandLine.begin() ); // delete the first "ot"
	} else {
		_warn("Command for processing is empty");
	}
	// mCommandLine = msg, sendfrom, alice, bob, hello
	_dbg1("Parsing: " << DbgVector(mCommandLine) );

	_dbg3("Alloc data");  
	mData = std::make_shared<cCmdData>();

	int phase=0; // 0: cmd name  1:var, 2:varExt  3:opt   9:end
	try {

		const string name = mCommandLine.at(0) + " " + mCommandLine.at(1) ; // "msg send"
		mFormat = mParser->FindFormat( name );
		_info("Got format for name="<<name);

		// msg send
		// msg ls
		// always 2 words are the command (we assume there are no sub-command)
		const size_t words_count = mCommandLine.size();
		const cCmdFormat & format = * mFormat; // const to be sure to just read from it (we are friends of this class)
		const size_t var_size_normal = format.mVar.size(); // number of the normal (mandatory) part of variables
		const size_t var_size_all = format.mVar.size() + format.mVarExt.size(); // number of the size of all variables (normal + extra)
		_dbg2("Format: size of vars: " << var_size_normal << " normal, and all is: " << var_size_all);
		int pos = 2; // "msg send"

		phase=1;
		const size_t offset_to_var = pos; // skip this many words before we have first var, to conver pos(word number) to var number

		if (phase==1) {
			while (true) { // parse var normal
				const int var_nr = pos - offset_to_var;
				_dbg2("phase="<<phase<<" pos="<<pos<<" var_nr="<<var_nr);
				if (pos >= words_count) { _dbg1("reached END, pos="<<pos);	phase=9; break;	}
				if (var_nr >= var_size_normal) { _dbg1("reached end of var normal, var_nr="<<var_nr); phase=2;	break;	}

				string word = mCommandLine.at(pos);
				_dbg1("phase="<<phase<<" pos="<<pos<<" word="<<word);
				++pos;

				if ( nUtils::CheckIfBegins("\"", word) ) { // TODO review memory access
					_dbg1("Quotes detected in: " + word);
					word.erase(0,1);
					while ( !nUtils::CheckIfEnds("\"", word) ) {
						word += " " + mCommandLine.at(pos);
						++pos;
					}
					word.erase(word.end(), word.end()-1); // ease the closing " of last mCommandLine[..] that is not at end of word
					_dbg1("Quoted word is:"<<word);
				}
				if (nUtils::CheckIfBegins("--", word)) { // --bcc foo
					phase=3; --pos; // this should be re-prased in proper phase
					_dbg1("Got an --option, so jumping to phase="<<phase);
					break; // continue to phase 3 - the options
				}

				_dbg1("adding var "<<word);  mData->mVar.push_back( word ); 
			}
		} // parse var phase 1

		if (phase==2) {
			while (true) { // parse var normal
				const int var_nr = pos - offset_to_var;
				_dbg2("phase="<<phase<<" pos="<<pos<<" var_nr="<<var_nr);
				if (pos >= words_count) { _dbg1("reached END, pos="<<pos);	phase=9; break;	}
				if (var_nr >= var_size_all) { _dbg1("reached end of var ALL, var_nr="<<var_nr); phase=3;	break;	}

				string word = mCommandLine.at(pos);
				_dbg1("phase="<<phase<<" pos="<<pos<<" word="<<word);
				++pos;

				if ( nUtils::CheckIfBegins("\"", word) ) { // TODO review memory access
					_dbg1("Quotes detected in: " + word);
					word.erase(0,1);
					while ( !nUtils::CheckIfEnds("\"", word) ) {
						word += " " + mCommandLine.at(pos);
						++pos;
					}
					word.erase(word.end(), word.end()-1); // ease the closing " of last mCommandLine[..] that is not at end of word
					_dbg1("Quoted word is:"<<word);
				}
				if (nUtils::CheckIfBegins("--", word)) { // --bcc foo
					phase=3; --pos; // this should be re-prased in proper phase
					_dbg1("Got an --option, so jumping to phase="<<phase);
					break; // continue to phase 3 - the options
				}

				_dbg1("adding var ext "<<word);  mData->mVarExt.push_back( word ); 
			}
		} // phase 2

		if (phase==3) {
			string prev_name="";  bool inside_opt=false; // are we now in middle of --option ?  curr_name is the opt name like "--cc"
			while (true) { // parse options
				if (pos >= words_count) { _dbg1("reached END, pos="<<pos);	phase=9; break;	}

				string word = mCommandLine.at(pos);
				_dbg1("phase="<<phase<<" pos="<<pos<<" word="<<word);
				++pos;

				bool is_newopt =  nUtils::CheckIfBegins("--", word); // word is opt name like "--cc"

				if (is_newopt) { // some new option like --private or --cc
					if (inside_opt) { // finish the previos option, that didn't got a value then.  --fast [--private]
						inside_opt=false;
						mData->AddOpt(prev_name , "");
						_dbg1("got option "<<prev_name<<" (empty)");
					}
					inside_opt=true; prev_name=word; // we now started the new option (and next iteration will finish it)
					_dbg3("started new option: prev_name="<<prev_name);
				}
				else { // not an --option, so should be a value to finish previous one
					if (inside_opt) { // we are in middle of option, now we have the argment that ends it: --cc [alice]
						string value=word; // like "alice"
						inside_opt=false;
						mData->AddOpt(prev_name , value);
						_dbg1("got option "<<prev_name<<" with value="<<value);
					}
					else { // we have a word like "bob", but we are not in middle of an option - syntax error
						throw cErrParseSyntax("Expected an --option here, but got a word=" + ToStr(word) + " at pos=" + ToStr(pos));
					}
				}
			} // all words
			if (inside_opt) { // finish the previos LAST option, that didn't got a value then.  --fast [--private] (END)
				inside_opt=false;
				mData->AddOpt(prev_name , "");
				_dbg1("got option "<<prev_name<<" (empty) - on end");
			}
		} // phase 3

		_note("mVar parsed:    " + DbgVector(mData->mVar));
		_note("mVarExt parsed: " + DbgVector(mData->mVarExt));
		_note("mOption parsed  " + DbgMap(mData->mOption));
 
	} catch (cErrParse &e) {
		_warn("Command can not be parsed " << e.what());
	}
}

vector<string> cCmdProcessing::UseComplete() {
	vector<string> ret;
	return ret;
}

void cCmdProcessing::UseExecute() {
	if (!mFormat) { _warn("Can not execute this command - mFormat is empty"); return; }
	cCmdExecutable exec = mFormat->getExec();
	exec( mData , *mUse ); 
}

// ========================================================================================================================

cParamInfo::cParamInfo(tFuncValid valid, tFuncHint hint) 
	: funcValid(valid), funcHint(hint)
{ }

// ========================================================================================================================

// cCmdFormat::cCmdFormat(cCmdExecutable exec, tVar var, tVar varExt, tOption opt) 
cCmdFormat::cCmdFormat(const cCmdExecutable &exec, const tVar &var, const tVar &varExt, const tOption &opt)
	:	mExec(exec), mVar(var), mVarExt(varExt), mOption(opt)
{
	_dbg1("Created new format");
}

cCmdExecutable cCmdFormat::getExec() const {
	return mExec;
}

void cCmdFormat::Debug() const {
	_info("Format at " << (void*)this );
	_info(".. mVar size=" << mVar.size());
	_info(".. mVarExt size=" << mVarExt.size());
	_info(".. mOption size=" << mOption.size());
}

// ========================================================================================================================

cCmdExecutable::tExitCode cCmdExecutable::operator()( shared_ptr<cCmdData> data, nUse::cUseOT & useOt) {
	_info("Executing function");
	int ret = mFunc( data , useOt );
	_info("Execution ret="<<ret);
	return ret;
}

cCmdExecutable::cCmdExecutable(tFunc func) : mFunc(func) { }

const cCmdExecutable::tExitCode cCmdExecutable::sSuccess = 0; 

// ========================================================================================================================

string cCmdData::VarAccess(int nr, const string &def, bool doThrow) const throw(cErrArgNotFound) { // see [nr] ; if doThrow then will throw on missing var, else returns def
	if (nr <= 0) throw cErrArgIllegal("Illegal number for var, nr="+ToStr(nr)+" (1,2,3... is expected)");
	const int ix = nr - 1;
	if (ix >= mVar.size()) { // then this is an extra argument
		const int ix_ext = ix - mVar.size();
		if (ix_ext >= mVarExt.size()) { // then this var number does not exist - out of range
			if (doThrow) {
				throw cErrArgMissing("Missing argument: out of range number for var, nr="+ToStr(nr)+" ix="+ToStr(ix)+" ix_ext="+ToStr(ix_ext)+" vs size="+ToStr(mVarExt.size()));
			}
			return def; // just return the default
		}
		return mVarExt.at(ix_ext);
	}
	return mVar.at(ix);
}

void cCmdData::AssertLegalOptName(const string &name) const throw(cErrArgIllegal) {
	if (name.size()<1) throw cErrArgIllegal("option name can not be empty");
	const size_t maxlen=100;
	if (name.size()>maxlen) throw cErrArgIllegal("option name too long, over" + ToStr(maxlen));
	// TODO test [a-zA-Z0-9_.-]*
}

vector<string> cCmdData::OptIf(const string& name) const throw(cErrArgIllegal) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { 
		return vector<string>{};
	} 
	return find->second;
}

string cCmdData::Opt1If(const string& name, const string &def) const throw(cErrArgIllegal) { // same but requires the 1st element; therefore we need def argument again
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { 
		return def;
	} 
	const auto &vec = find->second;
	if (vec.size()<1) { _warn("Not normalized opt for name="<<name); return def; }
	return vec.at(0);
}


string cCmdData::VarDef(int nr, const string &def, bool doThrow) const throw(cErrArgIllegal) {
	return VarAccess(nr, def, false);
}

string cCmdData::Var(int nr) const throw(cErrArgNotFound) { // nr: 1,2,3,4 including both arg and argExt
	static string nothing;
	return VarAccess(nr, nothing, true);
}

vector<string> cCmdData::Opt(const string& name) const throw(cErrArgNotFound) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { _warn("Map was: [TODO]"); throw cErrArgMissing("Option " + name + " was missing"); } 
	return find->second;
}

string cCmdData::Opt1(const string& name) const throw(cErrArgNotFound) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) {  throw cErrArgMissing("Option " + name + " was missing"); } 
	const auto &vec = find->second;
	if (vec.size()<1) { _warn("Not normalized opt for name="<<name); throw cErrArgMissing("Option " + name + " was missing (not-normalized empty vector)"); }
	return vec.at(0);
}

bool cCmdData::IsOpt(const string &name) const throw(cErrArgIllegal) {
	AssertLegalOptName(name);
	auto find = mOption.find( name );
	if (find == mOption.end()) { 
		return false; // no such option entry
	} 
	auto &vect = find->second;
	if (vect.size()) {
		return true; // yes, there is an option
	}

	_warn("Not normalized options for name="<<name<<" an empty vector exists there:" << DbgVector(vect));
	return false; // there was a vector for this options but it's empty now (maybe deleted?)
}

void cCmdData::AddOpt(const string &name, const string &value) throw(cErrArgIllegal) { // append an option with value (value can be empty
	_dbg3("adding option ["<<name<<"] with value="<<value);
	auto find = mOption.find( name );
	if (find == mOption.end()) {
		mOption.insert( std::make_pair( name , vector<string>{ value } ) );	
	} else {
		find->second.push_back( value );
	}	
}

// ========================================================================================================================


void cmd_test( shared_ptr<cUseOT> use ) {
	_mark("TEST TREE");

	shared_ptr<cCmdParser> parser(new cCmdParser);
	parser->Init();

	auto alltest = vector<string>{ ""
//	"ot msg ls"
//	,"ot msg ls alice"
	//,"ot msg sendfrom alice bob --prio 1"
	,"ot msg sendfrom alice bob --cc eve --cc mark --bcc john --prio 4"
//	,"ot msg sendfrom alice bob --cc eve --cc mark --bcc john --prio 4 --dryrun"
//	,"ot msg sendto bob hello --cc eve --cc mark --bcc john --prio 4"
//	,"ot msg rm alice 0"
//	,"ot msg-out rm alice 0"
		// to be tested
//	,"ot nym check alice"
//	,"ot nym info alice"
//	,"ot nym register alice"
//	,"ot nym rm alice"
//	,"ot nym new alice"
//	,"ot nym set-default alice"
//	,"ot nym refresh"
//	,"ot nym refresh alice"
//	,"ot nym ls"

//	,"ot account new assetname accountname"
//	,"ot refresh"
//	,"ot refresh accountname"
//	,"ot set-default accountname"
//	,"ot rm accountname"
//	,"ot ls"
//	,"ot mv accountname newaccountname"

//	,"ot asset new"
//	,"ot asset issue"
//	,"ot asset ls"

//	,"ot server add"
//	,"ot server ls"
//	,"ot server new"
//	,"ot server rm servername"
//	,"ot server set-default servername"

//	,"ot text encode texttoprocess"
//	,"ot text decode text"
//	,"ot text encrypt bob text"
//	,"ot text decrypt text"
	};
	for (auto cmd : alltest) {
		if (!cmd.length()) continue;
		_mark("====== Testing command: " << cmd );
		auto processing = parser->StartProcessing(cmd, use);
		processing.Parse();
		processing.UseExecute();
	}

}

}; // namespace nNewcli
}; // namespace OT


