'use babel';

import { register } from './mouseDetect'

export default {
  config: {
    back: {
      type: 'string',
      default: 'editor:move-to-beginning-of-previous-paragraph',
      enum: Object.keys(atom.commands.registeredCommands).sort()
    },
    forward: {
      type: 'string',
      default: 'editor:move-to-beginning-of-next-paragraph',
      enum: Object.keys(atom.commands.registeredCommands).sort()
    }
  },

  activate(state) {
    register(this.execute, atom.getCurrentWindow().getNativeWindowHandle())
  },

  execute(direction) {
    const activeElement = (document.activeElement === document.body) ? atom.views.getView(atom.workspace) : document.activeElement
    const command = atom.config.get(`mouse-history-buttons.${direction}`)
    const event = new CustomEvent(command, {bubbles: true, cancelable: true})
    activeElement.dispatchEvent(event)
  },

  deactivate() {
    // Can't deactivate apparently
  },
};
