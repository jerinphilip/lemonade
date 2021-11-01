#pragma once
#include "data.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "translator/annotation.h"
#include "translator/response.h"
#include "translator/response_options.h"

namespace lemonade {

#define LEMONADE_INLINE inline

///  JSONInterOperable
template <class T> void fromJSON(const std::string &json, T &out);
template <class T> std::string toJSON(const T &in);

LEMONADE_INLINE std::string asString(const rapidjson::Document &document) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  writer.SetFormatOptions(
      rapidjson::PrettyFormatOptions::kFormatSingleLineArray);
  document.Accept(writer);
  return buffer.GetString();
}

template <>
void LEMONADE_INLINE fromJSON<marian::bergamot::ResponseOptions>(
    const std::string &json, marian::bergamot::ResponseOptions &options) {}

template <>
LEMONADE_INLINE std::string toJSON<marian::bergamot::ResponseOptions>(
    const marian::bergamot::ResponseOptions &options) {
  rapidjson::Document document;
  document.SetObject();
  auto ator = document.GetAllocator();
  document.AddMember("qualityScores", options.qualityScores, ator);
  document.AddMember("alignment", options.alignment, ator);
  document.AddMember("sentenceMappings", options.sentenceMappings, ator);
  document.AddMember("concatStrategy", options.concatStrategy, ator);
  return asString(document);
}

template <>
LEMONADE_INLINE std::string
toJSON<marian::bergamot::Response>(const marian::bergamot::Response &response) {
  rapidjson::Document document;
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
  document.SetObject();

  auto addAnnotatedText =
      [&allocator](const marian::bergamot::AnnotatedText &in) {
        rapidjson::Value atext(rapidjson::kObjectType);
        atext.AddMember("text",
                        rapidjson::StringRef(in.text.data(), in.text.size()),
                        allocator);

        auto addAnnotation =
            [&allocator](const marian::bergamot::Annotation &annotation) {
              rapidjson::Value vAnnotation;
              vAnnotation.SetArray();
              for (size_t s = 0; s < annotation.numSentences(); s++) {
                rapidjson::Value vSentence;
                vSentence.SetArray();
                for (size_t w = 0; w < annotation.numWords(s); w++) {
                  rapidjson::Value vWord;
                  auto word = annotation.word(s, w);
                  vWord.SetArray();
                  vWord.PushBack(word.begin, allocator);
                  vWord.PushBack(word.end, allocator);
                  vSentence.PushBack(vWord, allocator);
                }
                vAnnotation.PushBack(vSentence, allocator);
              }
              return vAnnotation;
            };

        atext.AddMember("annotation", addAnnotation(in.annotation), allocator);
        return atext;
      };

  document.AddMember("source", addAnnotatedText(response.source), allocator);
  document.AddMember("target", addAnnotatedText(response.target), allocator);

  return asString(document);
}

template <>
LEMONADE_INLINE std::string toJSON<Payload>(const Payload &payload) {
  rapidjson::Document document;
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
  document.SetObject();

  document.AddMember(
      "source",
      rapidjson::StringRef(payload.source.data(), payload.source.size()),
      allocator);

  document.AddMember(
      "target",
      rapidjson::StringRef(payload.target.data(), payload.target.size()),
      allocator);

  document.AddMember(
      "query", rapidjson::StringRef(payload.query.data(), payload.query.size()),
      allocator);

  return asString(document);
}

template <>
LEMONADE_INLINE void fromJSON<Payload>(const std::string &json,
                                       Payload &payload) {
  rapidjson::Document document;
  document.Parse(json.c_str());
  payload.source = document["source"].GetString();
  payload.target = document["target"].GetString();
  payload.query = document["query"].GetString();
}

#undef LEMONADE_INLINE

} // namespace lemonade
