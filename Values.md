# Values

## 🏗️ Build the best way to develop user interfaces

This tells us what we are creating. It helps narrow our focus.

The best way to develop user interfaces is a _productive_ way of developing.

The best way to develop user interfaces creates _beautiful_ applications.

The best way to develop user interfaces creates _fast_ applications.

The best way to develop user interfaces enables developers to create applications _fast_.

The best way to develop user interfaces is _extensible_, so that we are not a barrier to developers.

## 🔎 Focus on the user and all else will follow

Our first priority is to our user.

Caring about the user means having high quality support for accessibility, top performance, stability, high fidelity and compatibility with the user's platform, supporting low-end devices, and so forth.

We respect our users, whoever they are.

Another way to phrase this is "Build a thing people want".

## 📖 Openness

gray is an open source project, in the full senses of the word: we are open to ideas, we are open to contributions, our code and our roadmap are open, our priorities are open. Transparency leads to a higher quality product.

## 💫 Maintaining quality

A mediocre product cannot be the best way to develop user interfaces, so we must build processes around maintaining high levels of quality.

This manifests in various ways. One is that we are feature-driven, not date-driven: we do not plan work based on deadlines. We may sometimes host events where we announce new features, but these events will announce features that have become available, rather than the features becoming available in order to be announced. This means sometimes a feature we intended to announce will slip and not be announced, but we prefer this to announcing a rushed feature.

We may sometimes gate features behind flags until we are confident of their quality.

## 🤣‬ Have fun doing it

Last, but definitely not least, we want to make sure that our work environment is pleasant for everyone involved. Your health and the health of your family and friends is more important than gray. Our community [is welcoming](CODE_OF_CONDUCT.md). We don't know everything; all of us can make mistakes.

We want team members to feel empowered to make changes to the code and to our processes.

We encourage a bias towards action. It's better to try something and be wrong, than to plan forever and never execute.

## Support

When we think about whether we claim to "support" something, e.g. whether gray supports Windows 7, we use the following frame of reference. We document [which platforms we consider to fall into each category](Supported_Platforms.md).

### Areas of support

We distinguish several categories along which one can describe a level of support:

* Supporting for deployment (we support a platform for deployment if you can run release builds of applications on that platform).
* Supporting for _development_ (we support a platform for development if you can run debug builds of applications on that platform, connect them to a debugger, hot reload, view logs, and so forth).
* Supporting for gray development (we support a platform for gray development if one can contribute to gray itself from that platform).

The list of [supported platforms](Supported_Platforms.md) is describing the platforms supported for deployment.

### Levels of support

For each area, we consider the level to which we provide support:

1. We will make a best effort to ensure that well written code works (e.g. we have testing on that platform). This is a common level for target platforms that have reached a label of "stable" on devices that are widely available.

1. We will not go out of our way to prevent code from working, but if it works it's because of circumstances and our best intentions rather than a coordinated effort (e.g. we do no testing on that platform). This is a common level of support for less commonly-used devices. This corresponds to the "Best effort platforms tested by the community" category on the list of supported platforms.

1. We will pay no attention to whether code works, but we will accept patches that are easy to reason about. This is a common level of support for the many esoteric embedded platforms that we have no way to even manually test. For example, if you have your own SoC and are running gray, we want you to succeed but we don't have any way to ensure it keeps working. At this level, we would not accept what we consider "risky" PRs, or PRs that involve significant refactorings. If you are willing to provide reliable CI support for your platform, we are more than happy to work with you to move that platform to level 2 (at which point more invasive PRs become viable).

1. We won't accept patches. This is the appropriate response for features and platforms that are wildly outside our roadmap. For example, maintaining a Rust port is not something the gray project would accept patches for. This corresponds to the "Unsupported platforms" category on the list of supported platforms.

_See also:_

* [Code of Conduct](CODE_OF_CONDUCT.md)
* [Contributor Guide](CONTRIBUTING.md)
