class TerminalCard extends HTMLElement {
  set hass(hass) {
    if (!this.content) {
      const card = document.createElement('ha-card');
      card.header = 'Terminal';
      this.content = document.createElement('div');
      this.content.style.padding = '0 16px 16px';
      this.content.style.height = '200px';
      this.content.style.overflow = 'auto';
      card.appendChild(this.content);
      this.appendChild(card);

      this.eventData = [];

      hass.connection.subscribeEvents((event) => {
        var date = new Date(event.time_fired);

        this.content.innerHTML += 
          "[" + date.toLocaleTimeString('en-US') + "]: " + event.data.method + ": " + event.data.gcode + "<br>";
          this.content.scrollTop = this.content.scrollHeight;
      }, 'esphome.gcode-event');
    }
  }

  setConfig(config) {
    this.config = config;
  }
}

customElements.define('terminal-card', TerminalCard);