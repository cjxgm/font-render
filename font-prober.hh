#pragma once
#include <fontconfig/fontconfig.h>
#include <vector>
#include <string>
#include <memory>


namespace font_prober
{
	struct finalizer
	{
		struct fini { ~fini() { FcFini(); } };
		static void enable()
		{
			static fini _;
		}
	};

	namespace propertys
	{
		inline namespace types
		{
			struct string
			{
				using raw_type = std::string;

				static raw_type get(FcPattern* pat, const char* object)
				{
					char* r;
					if (FcPatternGetString(pat, object, 0, (FcChar8**)&r) == FcResultMatch)
						return r;
					return {};
				}

				static void add(FcPattern* pat, const char* object, raw_type const& query)
				{
					FcPatternAddString(pat, object, (FcChar8 const*)query.c_str());
				}
			};

			struct integer
			{
				using raw_type = int;

				static raw_type get(FcPattern* pat, const char* object)
				{
					int r;
					if (FcPatternGetInteger(pat, object, 0, &r) == FcResultMatch)
						return r;
					return {};
				}

				static void add(FcPattern* pat, const char* object, raw_type const& query)
				{
					FcPatternAddInteger(pat, object, query);
				}
			};

			struct boolean
			{
				using raw_type = bool;

				static raw_type get(FcPattern* pat, const char* object)
				{
					FcBool r;
					if (FcPatternGetBool(pat, object, 0, &r) == FcResultMatch)
						return r;
					return {};
				}

				static void add(FcPattern* pat, const char* object, raw_type const& query)
				{
					FcPatternAddBool(pat, object, query);
				}
			};
		}

		template <class TAG, class TYPE>
		struct property
		{
			using value_type = typename TYPE::raw_type;

			property(value_type query) : query{std::move(query)} {}

			static auto get(FcPattern* pat) { return TYPE::get(pat, TAG::fc_property); }
			void add(FcPattern* pat) const { TYPE::add(pat, TAG::fc_property, query); }

		private:
			value_type const query;
		};

		namespace public_
		{
#define PROPERTY(TYPE, NAME, FNAME) \
			struct NAME : property<NAME, TYPE> \
			{ \
				using property::property; \
				static constexpr auto fc_property = FC_ ## FNAME; \
				struct storage { value_type NAME; }; \
			}

			PROPERTY( string, family, FAMILY);
			PROPERTY( string, file  ,   FILE);
			PROPERTY(integer, index ,  INDEX);
			PROPERTY( string, style ,  STYLE);
			PROPERTY( string, fullname, FULLNAME);
			PROPERTY(boolean, outline , OUTLINE );
			PROPERTY(boolean, scalable, SCALABLE);

#undef PROPERTY
		}
	}
	using namespace propertys::public_;

	template <class ...PROPERTYS>
	struct font_info : PROPERTYS::storage...
	{
		font_info(typename PROPERTYS::value_type... args)
			: PROPERTYS::storage{args}... {}
	};

	template <class ...QUERYS>
	inline auto query_pattern(FcPattern* pat, QUERYS const&... querys)
	{
#define EXPAND(EXPR)		(void)(int[]){ 0, ((EXPR), 0)... }
		EXPAND(querys.add(pat));
#undef EXPAND
		FcConfigSubstitute({}, pat, FcMatchPattern);
		FcDefaultSubstitute(pat);
	}

#define MAKE_UNIQUE(OBJ, FOBJ) \
	inline auto make_ ## OBJ (Fc ## FOBJ * x = nullptr) \
		-> std::unique_ptr<Fc ## FOBJ, decltype(Fc ## FOBJ ## Destroy)*> \
	{ \
		return {x, Fc ## FOBJ ## Destroy}; \
	}
	MAKE_UNIQUE(pattern, Pattern);
	MAKE_UNIQUE(font_set, FontSet);
	MAKE_UNIQUE(object_set, ObjectSet);
#undef MAKE_UNIQUE

	template <class ...PROPERTYS, class ...QUERYS>
	inline auto list(QUERYS const&... querys)
	{
		finalizer::enable();

		auto pat = make_pattern(FcPatternCreate());
		auto fonts = make_font_set();

		if (sizeof...(querys)) {
			query_pattern(&*pat, querys...);
			FcResult _;
			fonts.reset(FcFontSort({}, &*pat, false, {}, &_));
		}
		else {
			auto objs = make_object_set();
			objs.reset(FcObjectSetBuild(PROPERTYS::fc_property..., nullptr));
			fonts.reset(FcFontList({}, &*pat, &*objs));
		}

		std::vector<font_info<PROPERTYS...>> infos;
		for (int i=0; i<fonts->nfont; i++) {
			auto font = fonts->fonts[i];
			infos.emplace_back(PROPERTYS::get(font)...);
		}

		return infos;
	}

	template <class ...PROPERTYS, class ...QUERYS>
	inline auto pick(QUERYS const&... querys) -> font_info<PROPERTYS...>
	{
		finalizer::enable();

		auto pat = make_pattern(FcPatternCreate());
		query_pattern(&*pat, querys...);

		FcResult _;
		auto font = make_pattern(FcFontMatch({}, &*pat, &_));

		return {PROPERTYS::get(&*font)...};
	}
}

