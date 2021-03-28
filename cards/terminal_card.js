class TerminalCard extends HTMLElement {
  set hass(hass) {
    this._hass = hass;

    if (!this.console) {
      const card = document.createElement('ha-card');
      card.header = 'Terminal';

      this.console = document.createElement('div');
      this.console.style.padding = '0 16px 16px';
      this.console.style.height = '200px';
      this.console.style.overflow = 'auto';
      card.appendChild(this.console);

      this.form = document.createElement('form');
      this.input = document.createElement('input');
      this.input.type = 'text';
      this.input.style.width = '80%';
      this.form.appendChild(this.input);
      this.submit = document.createElement('input');
      this.submit.type = 'submit';
      this.submit.value = 'Send';
      this.submit.style.width = '18%';
      this.form.appendChild(this.submit);
      card.appendChild(this.form);

      this.appendChild(card);

      this.eventData = [];

      this.form.onsubmit = () => {
        const [domain, service] = this.config.service.split('.', 2);
        this._hass.callService(domain, service, {
          gcode: this.input.value
        });
        this.input.value = '';
        return false;
      };

      this._hass.connection.subscribeEvents((event) => {
        var date = new Date(event.time_fired);

        this.console.innerHTML += event.data.gcode.replaceAll('\n', "<br>");
          this.console.scrollTop = this.console.scrollHeight;
      }, this.config.event);
    }
  }

  setConfig(config) {
    if (!config.event) {
      throw new Error('You need to define event');
    }
    if (!config.service) {
      throw new Error('You need to define service');
    }

    this.config = config;
  }
}

customElements.define('terminal-card', TerminalCard);