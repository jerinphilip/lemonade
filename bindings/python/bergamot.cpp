#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <translator/annotation.h>
#include <translator/response.h>
#include <translator/response_options.h>
#include <translator/service.h>
#include <translator/translation_model.h>

#include <iostream>
#include <string>
#include <vector>

namespace py = pybind11;

using marian::bergamot::Alignment;
using marian::bergamot::AnnotatedText;
using marian::bergamot::ByteRange;
using marian::bergamot::ConcatStrategy;
using marian::bergamot::Point;
using marian::bergamot::Response;
using marian::bergamot::ResponseOptions;
using Service = marian::bergamot::AsyncService;
using _Model = marian::bergamot::TranslationModel;
using Model = std::shared_ptr<_Model>;

PYBIND11_MAKE_OPAQUE(std::vector<Response>);
PYBIND11_MAKE_OPAQUE(std::vector<std::string>);
PYBIND11_MAKE_OPAQUE(std::vector<Point>);
PYBIND11_MAKE_OPAQUE(std::vector<Alignment>);

// Nothing fancy; Super wasteful.  It is simply easier to do analysis in a
// Jupyter notebook, @jerinphilip is not doing efficiency here.

class ServicePyAdapter {
public:
  ServicePyAdapter(const Service::Config &config) {
    py::scoped_ostream_redirect outstream(
        std::cout,                                // std::ostream&
        py::module_::import("sys").attr("stdout") // Python output
    );
    py::scoped_ostream_redirect errstream(
        std::cerr,                                // std::ostream&
        py::module_::import("sys").attr("stderr") // Python output
    );

    py::call_guard<py::gil_scoped_release> gil_guard();
    service_.reset(std::move(new Service(config)));
  }

  Response translate(Model model, std::string input,
                     const ResponseOptions &options) {
    py::scoped_ostream_redirect outstream(
        std::cout,                                // std::ostream&
        py::module_::import("sys").attr("stdout") // Python output
    );
    py::scoped_ostream_redirect errstream(
        std::cerr,                                // std::ostream&
        py::module_::import("sys").attr("stderr") // Python output
    );
    py::call_guard<py::gil_scoped_release> gil_guard();

    std::promise<Response> responsePromise;
    std::future<Response> responseFuture = responsePromise.get_future();

    auto callback = [&responsePromise](Response &&response) {
      responsePromise.set_value(std::move(response));
    };

    service_->translate(model, std::move(input), std::move(callback), options);
    responseFuture.wait();
    return responseFuture.get();
  }

private:
  std::unique_ptr<Service> service_{nullptr};
};

PYBIND11_MODULE(pybergamot, m) {
  py::class_<ByteRange>(m, "ByteRange")
      .def(py::init<>())
      .def_readonly("begin", &ByteRange::begin)
      .def_readonly("end", &ByteRange::end)
      .def("__repr__", [](const ByteRange &range) {
        return "{" + std::to_string(range.begin) + ", " +
               std::to_string(range.end) + "}";
      });

  py::class_<AnnotatedText>(m, "AnnotatedText")
      .def(py::init<>())
      .def("numWords", &AnnotatedText::numWords)
      .def("numSentences", &AnnotatedText::numSentences)
      .def("word",
           [](const AnnotatedText &annotatedText, size_t sentenceIdx,
              size_t wordIdx) -> std::string {
             auto view = annotatedText.word(sentenceIdx, wordIdx);
             return std::string(view.data(), view.size());
           })
      .def("sentence",
           [](const AnnotatedText &annotatedText,
              size_t sentenceIdx) -> std::string {
             auto view = annotatedText.sentence(sentenceIdx);
             return std::string(view.data(), view.size());
           })
      .def("wordAsByteRange", &AnnotatedText::wordAsByteRange)
      .def("sentenceAsByteRange", &AnnotatedText::sentenceAsByteRange)
      .def_readonly("text", &AnnotatedText::text);

  py::class_<Response>(m, "Response")
      .def(py::init<>())
      .def_readonly("source", &Response::source)
      .def_readonly("target", &Response::target)
      .def_readonly("alignments", &Response::alignments);

  py::bind_vector<std::vector<Response>>(m, "VectorResponse");

  py::class_<ResponseOptions>(m, "ResponseOptions")
      .def(py::init<>())
      .def_readwrite("qualityScores", &ResponseOptions::qualityScores)
      .def_readwrite("alignment", &ResponseOptions::alignment)
      .def_readwrite("sentenceMappings", &ResponseOptions::sentenceMappings)
      .def_readwrite("alignmentThreshold",
                     &ResponseOptions::alignmentThreshold);

  py::enum_<ConcatStrategy>(m, "ConcatStrategy")
      .value("FAITHFUL", ConcatStrategy::FAITHFUL)
      .value("SPACE", ConcatStrategy::SPACE)
      .export_values();

  py::bind_vector<std::vector<std::string>>(m, "VectorString");
  py::class_<ServicePyAdapter>(m, "Service")
      .def(py::init<const Service::Config &>())
      .def("translate", &ServicePyAdapter::translate);

  py::class_<Service::Config>(m, "ServiceConfig")
      .def_readwrite("numWorkers", &Service::Config::numWorkers);

  py::class_<_Model, std::shared_ptr<_Model>>(m, "Model");
}
